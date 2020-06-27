#define GLFW_INCLUDE_VULKAN
#define STB_IMAGE_IMPLEMENTATION

#include "App.h"
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include "../Struct/Size2D.h"
#include "../Functions/callbacks.h"
#include "constants.h"
#include "InstanceFactory.h"
#include <vector>
#include <exception>
#include "../Functions/vulkan_ext.h"
#include "../Functions/control_flow.h"
#include "DeviceFactory.h"
#include "../Functions/factory.h"
#include <array>
#include "ShaderModuleFactory.h"
#include "DescriptorSetLayoutFactory.h"
#include "RenderPassFactory.h"
#include "GraphicsPipelineFactory.h"
#include "../Struct/Eye.h"
#include "../Struct/Drop.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Functions/math_ext.h"
#include "Enums.h"
#include <algorithm>
#include <thread>
#include <chrono>

using std::vector;
using std::exception;
using std::array;
using glm::vec2;
using glm::vec3;
using std::to_string;
using glm::perspective;
using glm::lookAt;
using std::min;
using std::this_thread::sleep_for;
using std::chrono::nanoseconds;

namespace ii887522::Rain
{
	App::App(const int maxHeight) : windowSize{ static_cast<int>(maxHeight * aspectRatio), maxHeight }, currentFrame{ 0u },
		swapchain{ VK_NULL_HANDLE }, rain{ new Rain }
	{
		glfwInit();
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
		window = glfwCreateWindow(windowSize.w, windowSize.h, "Rain", nullptr, nullptr);
		ico.pixels = stbi_load("res/drop.png", &ico.width, &ico.height, nullptr, STBI_rgb_alpha);
		configureWindow();
		initVulkan();
	}

	void App::configureWindow()
	{
		glfwSetWindowUserPointer(window, this);
		constexpr auto minHeight{ 360 };
		glfwSetWindowSizeLimits(window, static_cast<int>(minHeight * aspectRatio), minHeight, windowSize.w, windowSize.h);
		glfwSetCursorPosCallback(window, reactMouseMove);
		glfwSetWindowSizeCallback(window, reactWindowResize);
		glfwSetWindowIcon(window, 1, &ico);
	}

	void App::initVulkan()
	{
		instance = InstanceFactory{ }.make();
		if constexpr (isDebugging) initDebugMessenger();
		if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS) throw exception{ "Failed to create window surface!" };
		physicalDevice = getPhysicalDevice(*this);
		initDevice();
		descriptorSetLayout = DescriptorSetLayoutFactory{ }.make(device);
		initPipelineLayout();
		renderPass = RenderPassFactory{ }.make(device);
		graphicsPipeline = GraphicsPipelineFactory{ device, windowSize, pipelineLayout, renderPass }.make();
		uniformBufferAlignment = getUniformBufferAlignment(physicalDevice);
		initVertexBuffer(sizeof Drop::positions);
		initIndexBuffer(sizeof Drop::indices);
		const auto staticMemoryTypeIndex{ getMemoryTypeIndex(physicalDevice, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) };
		dynamicMemoryTypeIndex = getMemoryTypeIndex(physicalDevice, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_CACHED_BIT);
		const auto vertexBufferMemoryRequirements{ getBufferMemoryRequirements2(vertexBuffer) };
		const auto indexBufferMemoryRequirements{ getBufferMemoryRequirements2(indexBuffer) };
		staticMemory = allocateDeviceMemory(
			static_cast<unsigned int>(getAlignedSize(
				static_cast<unsigned int>(vertexBufferMemoryRequirements.memoryRequirements.size),
				static_cast<unsigned int>(indexBufferMemoryRequirements.memoryRequirements.alignment)
			) + indexBufferMemoryRequirements.memoryRequirements.size),
			staticMemoryTypeIndex
		);
		const VkBindBufferMemoryInfo bindBufferMemoryInfos[]{
			makeBindBufferMemoryInfo(vertexBuffer, staticMemory, 0u),
			makeBindBufferMemoryInfo(
				indexBuffer,
				staticMemory,
				getAlignedSize(
					static_cast<unsigned int>(vertexBufferMemoryRequirements.memoryRequirements.size),
					static_cast<unsigned int>(indexBufferMemoryRequirements.memoryRequirements.alignment)
				)
			)
		};
		if (vkBindBufferMemory2(device, sizeof bindBufferMemoryInfos / sizeof VkBindBufferMemoryInfo, bindBufferMemoryInfos) != VK_SUCCESS)
			throw exception{ "Failed to bind buffer to memory!" };
		initVulkanPart();
		initDeviceMemory();
		initImageAcquiredSemaphores();
		initRenderDoneSemaphores();
		initFrameFences();
	}

	void App::initDebugMessenger()
	{
		const auto debugCreateInfo{ makeDebugCreateInfo() };
		if (createDebugUtilsMessengerEXT(instance, &debugCreateInfo, nullptr, &debugMessenger) != VK_SUCCESS)
			throw exception{ "Failed to create debug messenger!" };
	}

	void App::initDevice()
	{
		const auto queueFamiliesProperties{ getQueueFamiliesProperties(physicalDevice) };
		const auto deviceQueuePriority{ 1.f };
		graphicsQueueFamilyIndex = getGraphicsQueueFamilyIndex(queueFamiliesProperties);
		presentQueueFamilyIndex = getPresentQueueFamilyIndex(physicalDevice, surface, queueFamiliesProperties);
		const auto deviceQueueCreateInfos{ makeDeviceQueueCreateInfos(deviceQueuePriority, graphicsQueueFamilyIndex, presentQueueFamilyIndex) };
		device = DeviceFactory{ deviceQueueCreateInfos }.make(physicalDevice);
		initGraphicsQueue();
		initPresentQueue();
	}

	void App::initGraphicsQueue()
	{
		const auto graphicsQueueInfo{ makeDeviceQueueInfo2(graphicsQueueFamilyIndex) };
		vkGetDeviceQueue2(device, &graphicsQueueInfo, &graphicsQueue);
	}

	void App::initPresentQueue()
	{
		const auto presentQueueInfo{ makeDeviceQueueInfo2(presentQueueFamilyIndex) };
		vkGetDeviceQueue2(device, &presentQueueInfo, &presentQueue);
	}

	void App::initVulkanPart(const Event event)
	{
		if (event == Event::NONE) swapchain = makeSwapchain();
		initSwapchainImages();
		initSwapchainImageViews();
		initSwapchainFramebuffers();
		initUniformBuffer(static_cast<unsigned int>(swapchainFramebuffers.size()) * getAlignedSize(sizeof Eye, uniformBufferAlignment));
		initInstanceBuffer(static_cast<unsigned int>(swapchainFramebuffers.size()) * dropCount * sizeof Drop::model);
		uniformBufferMemoryRequirements = getBufferMemoryRequirements2(uniformBuffer);
		instanceBufferMemoryRequirements = getBufferMemoryRequirements2(instanceBuffer);
		dynamicMemory = allocateDeviceMemory(
			static_cast<unsigned int>(getAlignedSize(
				static_cast<unsigned int>(uniformBufferMemoryRequirements.memoryRequirements.size),
				static_cast<unsigned int>(instanceBufferMemoryRequirements.memoryRequirements.alignment)
			) + instanceBufferMemoryRequirements.memoryRequirements.size),
			dynamicMemoryTypeIndex
		);
		const VkBindBufferMemoryInfo bindBufferMemoryInfos[]{
			makeBindBufferMemoryInfo(uniformBuffer, dynamicMemory, 0u),
			makeBindBufferMemoryInfo(
				instanceBuffer,
				dynamicMemory,
				getAlignedSize(
					static_cast<unsigned int>(uniformBufferMemoryRequirements.memoryRequirements.size),
					static_cast<unsigned int>(instanceBufferMemoryRequirements.memoryRequirements.alignment)
				)
			)
		};
		if (vkBindBufferMemory2(device, sizeof bindBufferMemoryInfos / sizeof VkBindBufferMemoryInfo, bindBufferMemoryInfos) != VK_SUCCESS)
			throw exception{ "Failed to bind buffer to memory!" };
		if (vkMapMemory(
			device, dynamicMemory, 0u,
			static_cast<unsigned int>(getAlignedSize(
				static_cast<unsigned int>(uniformBufferMemoryRequirements.memoryRequirements.size),
				static_cast<unsigned int>(instanceBufferMemoryRequirements.memoryRequirements.alignment)
			) + instanceBufferMemoryRequirements.memoryRequirements.size),
			0u, &dynamicData
		) != VK_SUCCESS) throw exception{ "Failed to map memory!" };
		initDescriptorPool();
		initDescriptorSets();
		initCommandPool();
		initCommandBuffers();
	}

	VkSwapchainKHR App::makeSwapchain() const
	{
		const array<uint32_t, 2u> queueFamilyIndices{ graphicsQueueFamilyIndex, presentQueueFamilyIndex };
		const auto swapchainCreateInfo{ makeSwapchainCreateInfoKHR(queueFamilyIndices) };
		VkSwapchainKHR l_swapchain;
		if (vkCreateSwapchainKHR(device, &swapchainCreateInfo, nullptr, &l_swapchain) != VK_SUCCESS)
			throw exception{ "Failed to create swap chain!" };
		return l_swapchain;
	}

	VkSwapchainCreateInfoKHR App::makeSwapchainCreateInfoKHR(const array<uint32_t, 2u>& queueFamilyIndices) const
	{
		VkSwapchainCreateInfoKHR swapchainCreateInfo{ };
		swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		swapchainCreateInfo.clipped = VK_TRUE;
		swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		swapchainCreateInfo.imageArrayLayers = 1u;
		swapchainCreateInfo.imageColorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
		swapchainCreateInfo.imageExtent = { static_cast<uint32_t>(windowSize.w), static_cast<uint32_t>(windowSize.h) };
		swapchainCreateInfo.imageFormat = VK_FORMAT_B8G8R8A8_SRGB;
		if (graphicsQueueFamilyIndex == presentQueueFamilyIndex) swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		else
		{
			swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices.data();
			swapchainCreateInfo.queueFamilyIndexCount = static_cast<uint32_t>(queueFamilyIndices.size());
		}
		swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		swapchainCreateInfo.minImageCount = 3u;
		swapchainCreateInfo.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
		swapchainCreateInfo.preTransform = getSurfaceCapabilities().surfaceCapabilities.currentTransform;
		swapchainCreateInfo.surface = surface;
		swapchainCreateInfo.oldSwapchain = swapchain;
		return swapchainCreateInfo;
	}

	VkSurfaceCapabilities2KHR App::getSurfaceCapabilities() const
	{
		VkSurfaceCapabilities2KHR surfaceCapabilities{ };
		surfaceCapabilities.sType = VK_STRUCTURE_TYPE_SURFACE_CAPABILITIES_2_KHR;
		VkPhysicalDeviceSurfaceInfo2KHR physicalDeviceSurfaceInfo{ };
		physicalDeviceSurfaceInfo.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_SURFACE_INFO_2_KHR;
		physicalDeviceSurfaceInfo.surface = surface;
		if (getPhysicalDeviceSurfaceCapabilities2KHR(instance, physicalDevice, &physicalDeviceSurfaceInfo, &surfaceCapabilities) != VK_SUCCESS)
			throw exception{ "Failed to get physical device surface capabilities!" };
		return surfaceCapabilities;
	}

	void App::initSwapchainImages()
	{
		uint32_t swapchainImageCount;
		if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, nullptr) != VK_SUCCESS)
			throw exception{ "Failed to get swap chain images!" };
		swapchainImages.resize(swapchainImageCount);
		if (vkGetSwapchainImagesKHR(device, swapchain, &swapchainImageCount, swapchainImages.data()) != VK_SUCCESS)
			throw exception{ "Failed to get swap chain images!" };
	}

	void App::initSwapchainImageViews()
	{
		swapchainImageViews.resize(swapchainImages.size());
		loop(static_cast<unsigned int>(swapchainImages.size()), [=](const auto i) {
			swapchainImageViews[i] = makeImageView(swapchainImages[i]);
			return Action::CONTINUE;
		});
	}

	VkImageView App::makeImageView(const VkImage image) const
	{
		const auto imageViewCreateInfo{ makeImageViewCreateInfo(image) };
		VkImageView imageView;
		if (vkCreateImageView(device, &imageViewCreateInfo, nullptr, &imageView) != VK_SUCCESS)
			throw exception{ "Failed to create image view!" };
		return imageView;
	}

	void App::initSwapchainFramebuffers()
	{
		swapchainFramebuffers.resize(swapchainImageViews.size());
		loop(static_cast<unsigned int>(swapchainImageViews.size()), [=](const auto i) {
			swapchainFramebuffers[i] = makeFramebuffer(swapchainImageViews[i]);
			return Action::CONTINUE;
		});
	}

	VkFramebuffer App::makeFramebuffer(const VkImageView imageView) const
	{
		const auto framebufferCreateInfo{ makeFramebufferCreateInfo(imageView) };
		VkFramebuffer framebuffer;
		if (vkCreateFramebuffer(device, &framebufferCreateInfo, nullptr, &framebuffer) != VK_SUCCESS)
			throw exception{ "Failed to create frame buffer!" };
		return framebuffer;
	}

	VkFramebufferCreateInfo App::makeFramebufferCreateInfo(const VkImageView& imageView) const
	{
		VkFramebufferCreateInfo framebufferCreateInfo{ };
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.attachmentCount = 1u;
		framebufferCreateInfo.pAttachments = &imageView;
		framebufferCreateInfo.width = windowSize.w;
		framebufferCreateInfo.height = windowSize.h;
		framebufferCreateInfo.layers = 1u;
		framebufferCreateInfo.renderPass = renderPass;
		return framebufferCreateInfo;
	}

	void App::initPipelineLayout()
	{
		const auto layoutCreateInfo{ makePipelineLayoutCreateInfo() };
		if (vkCreatePipelineLayout(device, &layoutCreateInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
			throw exception{ "Failed to create pipeline layout!" };
	}

	void App::initVertexBuffer(const unsigned int size)
	{
		const auto vertexBufferCreateInfo{ makeBufferCreateInfo(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT) };
		if (vkCreateBuffer(device, &vertexBufferCreateInfo, nullptr, &vertexBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create vertex buffer!" };
	}

	void App::initIndexBuffer(const unsigned int size)
	{
		const auto indexBufferCreateInfo{ makeBufferCreateInfo(size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT) };
		if (vkCreateBuffer(device, &indexBufferCreateInfo, nullptr, &indexBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create index buffer!" };
	}

	void App::initUniformBuffer(const unsigned int size)
	{
		const auto uniformBufferCreateInfo{ makeBufferCreateInfo(size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) };
		if (vkCreateBuffer(device, &uniformBufferCreateInfo, nullptr, &uniformBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create uniform buffer!" };
	}

	void App::initInstanceBuffer(const unsigned int size)
	{
		const auto instanceBufferCreateInfo{ makeBufferCreateInfo(size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) };
		if (vkCreateBuffer(device, &instanceBufferCreateInfo, nullptr, &instanceBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create instance buffer!" };
	}

	VkMemoryRequirements2 App::getBufferMemoryRequirements2(const VkBuffer buffer) const
	{
		VkBufferMemoryRequirementsInfo2 bufferMemoryRequirementsInfo{ };
		bufferMemoryRequirementsInfo.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_REQUIREMENTS_INFO_2;
		bufferMemoryRequirementsInfo.buffer = buffer;
		VkMemoryRequirements2 memoryRequirements{ };
		memoryRequirements.sType = VK_STRUCTURE_TYPE_MEMORY_REQUIREMENTS_2;
		vkGetBufferMemoryRequirements2(device, &bufferMemoryRequirementsInfo, &memoryRequirements);
		return memoryRequirements;
	}

	VkDeviceMemory App::allocateDeviceMemory(const unsigned int size, const unsigned int memoryTypeIndex) const
	{
		const auto memoryAllocateInfo{ makeMemoryAllocateInfo(size, memoryTypeIndex) };
		VkDeviceMemory deviceMemory;
		if (vkAllocateMemory(device, &memoryAllocateInfo, nullptr, &deviceMemory) != VK_SUCCESS)
			throw exception{ "Failed to allocate device memory!" };
		return deviceMemory;
	}

	void App::initDeviceMemory()
	{
		const auto stagingVertexBuffer{ makeStagingBuffer(sizeof Drop::positions) };
		const auto stagingIndexBuffer{ makeStagingBuffer(sizeof Drop::indices) };
		const auto stagingVertexBufferMemoryRequirements{ getBufferMemoryRequirements2(stagingVertexBuffer) };
		const auto stagingIndexBufferMemoryRequirements{ getBufferMemoryRequirements2(stagingIndexBuffer) };
		const auto stagingMemory{ allocateDeviceMemory(
			static_cast<unsigned int>(getAlignedSize(
				static_cast<unsigned int>(stagingVertexBufferMemoryRequirements.memoryRequirements.size),
				static_cast<unsigned int>(stagingIndexBufferMemoryRequirements.memoryRequirements.alignment)
			) + stagingIndexBufferMemoryRequirements.memoryRequirements.size),
			dynamicMemoryTypeIndex
		) };
		const VkBindBufferMemoryInfo bindStagingBufferStagingMemoryInfos[]{
			makeBindBufferMemoryInfo(stagingVertexBuffer, stagingMemory, 0u),
			makeBindBufferMemoryInfo(
				stagingIndexBuffer,
				stagingMemory,
				getAlignedSize(
					static_cast<unsigned int>(stagingVertexBufferMemoryRequirements.memoryRequirements.size),
					static_cast<unsigned int>(stagingIndexBufferMemoryRequirements.memoryRequirements.alignment)
				)
			)
		};
		if (vkBindBufferMemory2(
			device, sizeof bindStagingBufferStagingMemoryInfos / sizeof VkBindBufferMemoryInfo, bindStagingBufferStagingMemoryInfos
		) != VK_SUCCESS) throw exception{ "Failed to bind buffer to memory!" };
		initStagingMemory(stagingMemory, stagingVertexBufferMemoryRequirements, stagingIndexBufferMemoryRequirements);
		copyStagingMemoryToDeviceMemory(stagingVertexBuffer, stagingIndexBuffer);
		vkDestroyBuffer(device, stagingIndexBuffer, nullptr);
		vkDestroyBuffer(device, stagingVertexBuffer, nullptr);
		vkFreeMemory(device, stagingMemory, nullptr);
	}

	VkBuffer App::makeStagingBuffer(const unsigned int size)
	{
		const auto stagingBufferCreateInfo{ makeBufferCreateInfo(size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT) };
		VkBuffer stagingBuffer;
		if (vkCreateBuffer(device, &stagingBufferCreateInfo, nullptr, &stagingBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create staging buffer!" };
		return stagingBuffer;
	}

	VkCommandPool App::makeStagingCommandPool() const
	{
		const auto stagingCommandPoolCreateInfo{ makeStagingCommandPoolCreateInfo(graphicsQueueFamilyIndex) };
		VkCommandPool stagingCommandPool;
		if (vkCreateCommandPool(device, &stagingCommandPoolCreateInfo, nullptr, &stagingCommandPool) != VK_SUCCESS)
			throw exception{ "Failed to create staging command pool!" };
		return stagingCommandPool;
	}

	VkCommandBuffer App::makeStagingCommandBuffer(const VkCommandPool stagingCommandPool) const
	{
		const auto stagingCommandBufferAllocateInfo{ makeStagingCommandBufferAllocateInfo(stagingCommandPool) };
		VkCommandBuffer stagingCommandBuffer;
		if (vkAllocateCommandBuffers(device, &stagingCommandBufferAllocateInfo, &stagingCommandBuffer) != VK_SUCCESS)
			throw exception{ "Failed to create staging command buffer!" };
		return stagingCommandBuffer;
	}

	void App::initStagingMemory(const VkDeviceMemory stagingMemory, const VkMemoryRequirements2 left, const VkMemoryRequirements2 right) const
	{
		void* data;
		if (vkMapMemory(
			device, stagingMemory, 0u,
			getAlignedSize(
				static_cast<unsigned int>(left.memoryRequirements.size), static_cast<unsigned int>(right.memoryRequirements.alignment)
			) + right.memoryRequirements.size,
			0u, &data
		) != VK_SUCCESS) throw exception{ "Failed to map memory!" };
		constexpr Drop drop;
		*reinterpret_cast<array<vec2, 4u>*>(data) = drop.positions;
		*reinterpret_cast<array<unsigned int, 6u>*>(reinterpret_cast<char*>(data) + getAlignedSize(
			static_cast<unsigned int>(left.memoryRequirements.size), static_cast<unsigned int>(right.memoryRequirements.alignment)
		)) = drop.indices;
		const auto mappedStagingMemoryRange{ makeMappedMemoryRange(
			stagingMemory,
			0u,
			getAlignedSize(
				static_cast<unsigned int>(left.memoryRequirements.size), static_cast<unsigned int>(right.memoryRequirements.alignment)
			)
		) };
		if (vkFlushMappedMemoryRanges(device, 1u, &mappedStagingMemoryRange) != VK_SUCCESS)
			throw exception{ "Failed to flush mapped memory ranges!" };
		vkUnmapMemory(device, stagingMemory);
	}

	void App::copyStagingMemoryToDeviceMemory(const VkBuffer stagingVertexBuffer, const VkBuffer stagingIndexBuffer) const
	{
		const auto stagingCommandPool{ makeStagingCommandPool() };
		const auto stagingCommandBuffer{ makeStagingCommandBuffer(stagingCommandPool) };
		beginImmediateStagingCommandBufferSync(stagingCommandBuffer);
		const auto vertexBufferCopyRegion{ makeBufferCopyRegion(sizeof Drop::positions) };
		vkCmdCopyBuffer(stagingCommandBuffer, stagingVertexBuffer, vertexBuffer, 1u, &vertexBufferCopyRegion);
		const auto indexBufferCopyRegion{ makeBufferCopyRegion(sizeof Drop::indices) };
		vkCmdCopyBuffer(stagingCommandBuffer, stagingIndexBuffer, indexBuffer, 1u, &indexBufferCopyRegion);
		endImmediateStagingCommandBufferSync(stagingCommandBuffer);
		vkFreeCommandBuffers(device, stagingCommandPool, 1u, &stagingCommandBuffer);
		vkDestroyCommandPool(device, stagingCommandPool, nullptr);
	}

	void App::beginImmediateStagingCommandBufferSync(const VkCommandBuffer stagingCommandBuffer) const
	{
		VkCommandBufferBeginInfo stagingCommandBufferBeginInfo{ };
		stagingCommandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		stagingCommandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		if (vkBeginCommandBuffer(stagingCommandBuffer, &stagingCommandBufferBeginInfo) != VK_SUCCESS)
			throw exception{ "Failed to begin recording staging command buffer!" };
	}

	void App::endImmediateStagingCommandBufferSync(const VkCommandBuffer stagingCommandBuffer) const
	{
		if (vkEndCommandBuffer(stagingCommandBuffer) != VK_SUCCESS) throw exception{ "Failed to record staging command buffer!" };
		const auto stagingSubmitInfo{ makeStagingSubmitInfo(stagingCommandBuffer) };
		vkQueueSubmit(graphicsQueue, 1u, &stagingSubmitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);
	}

	void App::initDescriptorPool()
	{
		const auto descriptorPoolSize{ makeDescriptorPoolSize() };
		const auto descriptorPoolCreateInfo{ makeDescriptorPoolCreateInfo(descriptorPoolSize) };
		if (vkCreateDescriptorPool(device, &descriptorPoolCreateInfo, nullptr, &descriptorPool) != VK_SUCCESS)
			throw exception{ "Failed to create descriptor pool!" };
	}

	VkDescriptorPoolCreateInfo App::makeDescriptorPoolCreateInfo(const VkDescriptorPoolSize& descriptorPoolSize) const
	{
		VkDescriptorPoolCreateInfo descriptorPoolCreateInfo{ };
		descriptorPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		descriptorPoolCreateInfo.maxSets = static_cast<uint32_t>(swapchainFramebuffers.size());
		descriptorPoolCreateInfo.poolSizeCount = 1u;
		descriptorPoolCreateInfo.pPoolSizes = &descriptorPoolSize;
		return descriptorPoolCreateInfo;
	}

	VkDescriptorPoolSize App::makeDescriptorPoolSize() const
	{
		VkDescriptorPoolSize descriptorPoolSize{ };
		descriptorPoolSize.descriptorCount = static_cast<uint32_t>(swapchainFramebuffers.size());
		descriptorPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		return descriptorPoolSize;
	}

	void App::initDescriptorSets()
	{
		descriptorSets.resize(swapchainFramebuffers.size());
		const vector<VkDescriptorSetLayout> descriptorSetLayouts(swapchainFramebuffers.size(), descriptorSetLayout);
		const auto descriptorSetAllocateInfo{ makeDescriptorSetAllocateInfo(descriptorSetLayouts) };
		if (vkAllocateDescriptorSets(device, &descriptorSetAllocateInfo, descriptorSets.data()) != VK_SUCCESS)
			throw exception{ "Failed to allocate descriptor sets!" };
		updateDescriptorSets();
	}

	VkDescriptorSetAllocateInfo App::makeDescriptorSetAllocateInfo(const vector<VkDescriptorSetLayout>& descriptorSetLayouts) const
	{
		VkDescriptorSetAllocateInfo descriptorSetAllocateInfo{ };
		descriptorSetAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptorSetAllocateInfo.descriptorPool = descriptorPool;
		descriptorSetAllocateInfo.descriptorSetCount = static_cast<uint32_t>(descriptorSetLayouts.size());
		descriptorSetAllocateInfo.pSetLayouts = descriptorSetLayouts.data();
		return descriptorSetAllocateInfo;
	}

	void App::updateDescriptorSets()
	{
		const auto descriptorBufferInfos{ makeDescriptorBufferInfos() };
		const auto descriptorSetWrites{ makeDescriptorSetWrites(descriptorBufferInfos) };
		vkUpdateDescriptorSets(device, static_cast<uint32_t>(descriptorSetWrites.size()), descriptorSetWrites.data(), 0u, nullptr);
	}

	vector<VkDescriptorBufferInfo> App::makeDescriptorBufferInfos() const
	{
		vector<VkDescriptorBufferInfo> descriptorBufferInfos;
		descriptorBufferInfos.reserve(descriptorSets.size());
		loop(static_cast<unsigned int>(descriptorSets.size()), [&](const auto i) {
			descriptorBufferInfos.push_back(makeDescriptorBufferInfo(i));
			return Action::CONTINUE;
		});
		return descriptorBufferInfos;
	}

	vector<VkWriteDescriptorSet> App::makeDescriptorSetWrites(const vector<VkDescriptorBufferInfo>& descriptorBufferInfos) const
	{
		vector<VkWriteDescriptorSet> descriptorSetWrites;
		descriptorSetWrites.reserve(descriptorSets.size());
		loop(static_cast<unsigned int>(descriptorSets.size()), [&](const auto i) {
			descriptorSetWrites.push_back(makeDescriptorSetWrite(descriptorSets[i], descriptorBufferInfos[i]));
			return Action::CONTINUE;
		});
		return descriptorSetWrites;
	}

	VkDescriptorBufferInfo App::makeDescriptorBufferInfo(const unsigned int i) const
	{
		VkDescriptorBufferInfo descriptorBufferInfo{ };
		descriptorBufferInfo.buffer = uniformBuffer;
		descriptorBufferInfo.offset = i * getAlignedSize(sizeof Eye, uniformBufferAlignment);
		descriptorBufferInfo.range = sizeof Eye;
		return descriptorBufferInfo;
	}

	void App::initCommandPool()
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{ };
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		if (vkCreateCommandPool(device, &commandPoolCreateInfo, nullptr, &commandPool) != VK_SUCCESS)
			throw exception{ "Failed to create command pool!" };
	}

	void App::initCommandBuffers()
	{
		commandBuffers.resize(descriptorSets.size());
		const auto commandBufferAllocateInfo{ makeCommandBufferAllocateInfo() };
		if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo, commandBuffers.data()) != VK_SUCCESS)
			throw exception{ "Failed to allocate command buffers!" };
		recordCommandBuffers();
	}

	VkCommandBufferAllocateInfo App::makeCommandBufferAllocateInfo() const
	{
		VkCommandBufferAllocateInfo commandBufferAllocateInfo{ };
		commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(descriptorSets.size());
		commandBufferAllocateInfo.commandPool = commandPool;
		commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		return commandBufferAllocateInfo;
	}

	void App::recordCommandBuffers() const
	{
		loop(static_cast<unsigned int>(commandBuffers.size()), [=](const auto i) {
			recordCommandBuffer(i);
			return Action::CONTINUE;
		});
	}

	void App::recordCommandBuffer(const unsigned int i) const
	{
		VkCommandBufferBeginInfo commandBufferBeginInfo{ };
		commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		vkBeginCommandBuffer(commandBuffers[i], &commandBufferBeginInfo);
		const auto viewport{ makeViewport(windowSize) };
		vkCmdSetViewport(commandBuffers[i], 0u, 1u, &viewport);
		const auto scissor{ makeScissor(windowSize) };
		vkCmdSetScissor(commandBuffers[i], 0u, 1u, &scissor);
		beginRenderPass(i);
		vkCmdBindDescriptorSets(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout, 0u, 1u, &descriptorSets[i], 0u, nullptr);
		vkCmdBindIndexBuffer(commandBuffers[i], indexBuffer, 0u, VK_INDEX_TYPE_UINT32);
		vkCmdBindPipeline(commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);
		const VkBuffer vertexBuffers[]{ vertexBuffer, instanceBuffer };
		const VkDeviceSize offsets[]{ 0u, 0u };
		vkCmdBindVertexBuffers(commandBuffers[i], 0u, sizeof vertexBuffers / sizeof VkBuffer, vertexBuffers, offsets);
		vkCmdDrawIndexed(commandBuffers[i], sizeof Drop::indices / sizeof(unsigned int), dropCount, 0u, 0, 0u);
		endRenderPass(i);
		vkEndCommandBuffer(commandBuffers[i]);
	}

	void App::beginRenderPass(const unsigned int i) const
	{
		const VkClearValue clearColor{ 0.f, 0.f, 0.f, 1.f };
		const auto renderPassBeginInfo{ makeRenderPassBeginInfo(clearColor, swapchainFramebuffers[i]) };
		VkSubpassBeginInfo subpassBeginInfo{ };
		subpassBeginInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_BEGIN_INFO;
		subpassBeginInfo.contents = VK_SUBPASS_CONTENTS_INLINE;
		cmdBeginRenderPass2KHR(device, commandBuffers[i], &renderPassBeginInfo, &subpassBeginInfo);
	}

	void App::endRenderPass(const unsigned int i) const
	{
		VkSubpassEndInfo subpassEndInfo{ };
		subpassEndInfo.sType = VK_STRUCTURE_TYPE_SUBPASS_END_INFO;
		cmdEndRenderPass2KHR(device, commandBuffers[i], &subpassEndInfo);
	}

	VkRenderPassBeginInfo App::makeRenderPassBeginInfo(const VkClearValue& clearColor, const VkFramebuffer swapchainFramebuffer) const
	{
		VkRenderPassBeginInfo renderPassBeginInfo{ };
		renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassBeginInfo.clearValueCount = 1u;
		renderPassBeginInfo.pClearValues = &clearColor;
		renderPassBeginInfo.framebuffer = swapchainFramebuffer;
		renderPassBeginInfo.renderArea.offset = { 0, 0 };
		renderPassBeginInfo.renderArea.extent = { static_cast<uint32_t>(windowSize.w), static_cast<uint32_t>(windowSize.h) };
		renderPassBeginInfo.renderPass = renderPass;
		return renderPassBeginInfo;
	}

	void App::initImageAcquiredSemaphores()
	{
		loop(maxFramesInFlight, [=](const auto i) {
			imageAcquiredSemaphores[i] = makeSemaphore();
			return Action::CONTINUE;
		});
	}

	void App::initRenderDoneSemaphores()
	{
		loop(maxFramesInFlight, [=](const auto i) {
			renderDoneSemaphores[i] = makeSemaphore();
			return Action::CONTINUE;
		});
	}

	void App::initFrameFences()
	{
		loop(maxFramesInFlight, [=](const auto i) {
			frameFences[i] = makeFence();
			return Action::CONTINUE;
		});
	}

	VkSemaphore App::makeSemaphore() const
	{
		VkSemaphoreCreateInfo sempahoreCreateInfo{ };
		sempahoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VkSemaphore semaphore;
		if (vkCreateSemaphore(device, &sempahoreCreateInfo, nullptr, &semaphore) != VK_SUCCESS)
			throw exception{ "Failed to create semaphore!" };
		return semaphore;
	}

	VkFence App::makeFence() const
	{
		VkFenceCreateInfo fenceCreateInfo{ };
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		VkFence fence;
		if (vkCreateFence(device, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS) throw exception{ "Failed to create fence!" };
		return fence;
	}

	vector<VkPhysicalDevice> App::getPhysicalDevices() const
	{
		uint32_t physicalDeviceCount;
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, nullptr) != VK_SUCCESS)
			throw exception{ "Failed to enumerate physical devices!" };
		vector<VkPhysicalDevice> physicalDevices(physicalDeviceCount);
		if (vkEnumeratePhysicalDevices(instance, &physicalDeviceCount, physicalDevices.data()) != VK_SUCCESS)
			throw exception{ "Failed to enumerate physical devices!" };
		return physicalDevices;
	}

	void App::eventLoop()
	{
		glfwShowWindow(window);
		auto frameCount{ 0u };
		auto elaspedTime{ 0.f };
		auto lastTime{ glfwGetTime() };
		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			const auto now{ glfwGetTime() };
			const auto dt{ static_cast<float>(now - lastTime) };
			elaspedTime += dt;
			lastTime = now;
			if (elaspedTime >= 1.f)
			{
				glfwSetWindowTitle(window, ("Rain: " + to_string(frameCount) + " FPS").c_str());
				frameCount = 0u;
				elaspedTime = 0.f;
			}
			run(min(dt, .06667f));
			++frameCount;
			sleep_for(nanoseconds{ 1500000 });
		}
		vkDeviceWaitIdle(device);
	}

	void App::run(const float dt)
	{
		syncFence(frameFences[currentFrame]);
		const auto imageIndex{ acquireImageAsync() };
		step(imageIndex, dt);
		renderAsync(imageIndex);
		releaseImageAsync(imageIndex);
		currentFrame = (currentFrame + 1) % maxFramesInFlight;
	}

	void App::syncFence(const VkFence fence) const
	{
		if (vkWaitForFences(device, 1u, &fence, VK_FALSE, UINT64_MAX) != VK_SUCCESS)
			throw exception{ "Failed to wait for fences!" };
		if (vkResetFences(device, 1u, &fence) != VK_SUCCESS) throw exception{ "Failed to reset fences!" };
	}
	
	uint32_t App::acquireImageAsync()
	{
		const auto acquireNextImageInfo{ makeAcquireNextImageInfo() };
		uint32_t imageIndex;
		switch (vkAcquireNextImage2KHR(device, &acquireNextImageInfo, &imageIndex))
		{
		case VK_SUCCESS:
			return imageIndex;
		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			while (windowSize.w == 0 || windowSize.h == 0) glfwWaitEvents();
			vkDeviceWaitIdle(device);
			destroyVulkanPart(Event::CREATE_SWAPCHAIN);
			initVulkanPart(Event::SWAPCHAIN_RECREATED);
			return acquireImageAsync();
		default:
			throw exception{ "Failed to acquire next image!" };
		}
	}

	void App::step(const uint32_t imageIndex, const float dt)
	{
		rain->fall(dt);
		*reinterpret_cast<Eye*>(reinterpret_cast<char*>(dynamicData) + imageIndex * getAlignedSize(sizeof Eye, uniformBufferAlignment)) = Eye{
			perspective(90.f / aspectRatio, aspectRatio, 1.f, 75.f),
			lookAt(vec3{ 0.f, 0.f, 0.f }, vec3{ 0.f, 0.f, -1.f }, vec3{ 0.f, 1.f, 0.f })
		};
		rain->serializeModels(
			reinterpret_cast<char*>(dynamicData) + getAlignedSize(
				static_cast<unsigned int>(uniformBufferMemoryRequirements.memoryRequirements.size),
				static_cast<unsigned int>(instanceBufferMemoryRequirements.memoryRequirements.alignment)
			) + imageIndex * dropCount * sizeof Drop::model
		);
		const VkMappedMemoryRange mappedMemoryRanges[]{
			makeMappedMemoryRange(dynamicMemory, imageIndex * getAlignedSize(sizeof Eye, uniformBufferAlignment), sizeof Eye),
			makeMappedMemoryRange(dynamicMemory, imageIndex * dropCount * sizeof Drop::model, dropCount * sizeof Drop::model)
		};
		if (vkFlushMappedMemoryRanges(device, sizeof mappedMemoryRanges / sizeof VkMappedMemoryRange, mappedMemoryRanges) != VK_SUCCESS)
			throw exception{ "Failed to flush mapped memory ranges!" };
	}

	VkSubmitInfo App::makeSubmitInfo(const uint32_t imageIndex, const VkPipelineStageFlags& waitStage) const
	{
		VkSubmitInfo submitInfo{ };
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1u;
		submitInfo.pCommandBuffers = &commandBuffers[imageIndex];
		submitInfo.signalSemaphoreCount = 1u;
		submitInfo.pSignalSemaphores = &renderDoneSemaphores[currentFrame];
		submitInfo.waitSemaphoreCount = 1u;
		submitInfo.pWaitSemaphores = &imageAcquiredSemaphores[currentFrame];
		submitInfo.pWaitDstStageMask = &waitStage;
		return submitInfo;
	}

	void App::renderAsync(const uint32_t imageIndex) const
	{
		const auto waitStage{ VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		const auto submitInfo{ makeSubmitInfo(imageIndex, waitStage) };
		if (vkQueueSubmit(graphicsQueue, 1u, &submitInfo, frameFences[currentFrame]) != VK_SUCCESS)
			throw exception{ "Failed to submit to graphics queue!" };
	}

	void App::releaseImageAsync(const uint32_t imageIndex)
	{
		const auto presentInfo{ makePresentInfo(imageIndex) };
		switch (vkQueuePresentKHR(presentQueue, &presentInfo))
		{
		case VK_SUCCESS:
			break;
		case VK_SUBOPTIMAL_KHR:
		case VK_ERROR_OUT_OF_DATE_KHR:
			while (windowSize.w == 0 || windowSize.h == 0) glfwWaitEvents();
			vkDeviceWaitIdle(device);
			destroyVulkanPart(Event::CREATE_SWAPCHAIN);
			initVulkanPart(Event::SWAPCHAIN_RECREATED);
			break;
		default:
			throw exception{ "Failed to submit to present queue!" };
		}
	}

	void App::destroyFrameFences() const
	{
		loop(maxFramesInFlight, [=](const auto i) {
			vkDestroyFence(device, frameFences[i], nullptr);
			return Action::CONTINUE;
		});
	}

	void App::destroyRenderDoneSemaphores() const
	{
		loop(maxFramesInFlight, [=](const auto i) {
			vkDestroySemaphore(device, renderDoneSemaphores[i], nullptr);
			return Action::CONTINUE;
		});
	}

	void App::destroyImageAcquiredSemaphores() const
	{
		loop(maxFramesInFlight, [=](const auto i) {
			vkDestroySemaphore(device, imageAcquiredSemaphores[i], nullptr);
			return Action::CONTINUE;
		});
	}

	void App::destroyVulkanPart(const Event event)
	{
		vkFreeCommandBuffers(device, commandPool, static_cast<uint32_t>(commandBuffers.size()), commandBuffers.data());
		vkDestroyCommandPool(device, commandPool, nullptr);
		vkDestroyDescriptorPool(device, descriptorPool, nullptr);
		vkUnmapMemory(device, dynamicMemory);
		vkFreeMemory(device, dynamicMemory, nullptr);
		vkDestroyBuffer(device, instanceBuffer, nullptr);
		vkDestroyBuffer(device, uniformBuffer, nullptr);
		destroySwapchainFramebuffers();
		destroySwapchainImageViews();
		if (event == Event::CREATE_SWAPCHAIN)
		{
			const auto l_swapchain{ makeSwapchain() };
			vkDestroySwapchainKHR(device, swapchain, nullptr);
			swapchain = l_swapchain;
		}
		else vkDestroySwapchainKHR(device, swapchain, nullptr);
	}

	void App::destroySwapchainFramebuffers() const
	{
		for (const auto swapchainFramebuffer : swapchainFramebuffers) vkDestroyFramebuffer(device, swapchainFramebuffer, nullptr);
	}

	void App::destroySwapchainImageViews() const
	{
		for (const auto swapchainImageView : swapchainImageViews) vkDestroyImageView(device, swapchainImageView, nullptr);
	}

	App::~App()
	{
		delete rain;
		destroyFrameFences();
		destroyRenderDoneSemaphores();
		destroyImageAcquiredSemaphores();
		destroyVulkanPart();
		vkDestroyBuffer(device, indexBuffer, nullptr);
		vkDestroyBuffer(device, vertexBuffer, nullptr);
		vkFreeMemory(device, staticMemory, nullptr);
		vkDestroyPipeline(device, graphicsPipeline, nullptr);
		vkDestroyRenderPass(device, renderPass, nullptr);
		vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
		vkDestroyDescriptorSetLayout(device, descriptorSetLayout, nullptr);
		vkDestroyDevice(device, nullptr);
		vkDestroySurfaceKHR(instance, surface, nullptr);
		if constexpr (isDebugging) destroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		vkDestroyInstance(instance, nullptr);
		stbi_image_free(ico.pixels);
		glfwDestroyWindow(window);
		glfwTerminate();
	}
}