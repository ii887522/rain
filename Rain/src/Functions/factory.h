#ifndef II887522_RAIN_FACTORY_H
#define II887522_RAIN_FACTORY_H

#include <vulkan/vulkan.h>
#include "callbacks.h"
#include "../Any/constants.h"
#include <vector>
#include <glm/glm.hpp>
#include <array>
#include "control_flow.h"
#include "../Struct/Size2D.h"

using std::vector;
using glm::vec2;
using glm::mat4;
using glm::vec4;
using std::array;

namespace ii887522::Rain
{
	constexpr VkApplicationInfo makeAppInfo()
	{
		VkApplicationInfo appInfo{ };
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.apiVersion = VK_API_VERSION_1_2;
		appInfo.pApplicationName = "Rain";
		appInfo.applicationVersion = VK_MAKE_VERSION(1u, 0u, 0u);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1u, 0u, 0u);
		return appInfo;
	}

	constexpr VkDebugUtilsMessengerCreateInfoEXT makeDebugCreateInfo()
	{
		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{ };
		debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT |
			VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		debugCreateInfo.pfnUserCallback = debugLog;
		return debugCreateInfo;
	}

	VKAPI_ATTR VkResult VKAPI_CALL createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger);
	VKAPI_ATTR void VKAPI_CALL destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
		const VkAllocationCallbacks* pAllocator);

	constexpr VkDeviceQueueCreateInfo makeDeviceQueueCreateInfo(const float& priority, const uint32_t queueFamilyIndex)
	{
		VkDeviceQueueCreateInfo deviceQueueCreateInfo{ };
		deviceQueueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		deviceQueueCreateInfo.pQueuePriorities = &priority;
		deviceQueueCreateInfo.queueCount = 1u;
		deviceQueueCreateInfo.queueFamilyIndex = queueFamilyIndex;
		return deviceQueueCreateInfo;
	}

	vector<VkDeviceQueueCreateInfo> makeDeviceQueueCreateInfos(const float& priority, const uint32_t graphicsQueueFamilyIndex,
		const uint32_t presentQueueFamilyIndex);

	constexpr VkDeviceQueueInfo2 makeDeviceQueueInfo2(const uint32_t queueFamilyIndex)
	{
		VkDeviceQueueInfo2 deviceQueueInfo{ };
		deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_INFO_2;
		deviceQueueInfo.queueFamilyIndex = queueFamilyIndex;
		deviceQueueInfo.queueIndex = 0u;
		return deviceQueueInfo;
	}

	constexpr VkImageViewCreateInfo makeImageViewCreateInfo(const VkImage image)
	{
		VkImageViewCreateInfo imageViewCreateInfo{ };
		imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		imageViewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
		imageViewCreateInfo.format = VK_FORMAT_B8G8R8A8_SRGB;
		imageViewCreateInfo.image = image;
		imageViewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0u;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0u;
		imageViewCreateInfo.subresourceRange.layerCount = 1u;
		imageViewCreateInfo.subresourceRange.levelCount = 1u;
		imageViewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		return imageViewCreateInfo;
	}

	constexpr VkPipelineShaderStageCreateInfo makePipelineShaderStageCreateInfo(const VkShaderModule shaderModule,
		const VkShaderStageFlagBits stage)
	{
		VkPipelineShaderStageCreateInfo shaderStageCreateInfo{ };
		shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageCreateInfo.module = shaderModule;
		shaderStageCreateInfo.pName = "main";
		shaderStageCreateInfo.stage = stage;
		return shaderStageCreateInfo;
	}

	constexpr VkVertexInputBindingDescription makePositionBindingDescription()
	{
		VkVertexInputBindingDescription positionBindingDescription{ };
		positionBindingDescription.binding = 0u;
		positionBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		positionBindingDescription.stride = sizeof vec2;
		return positionBindingDescription;
	}

	constexpr VkVertexInputBindingDescription makeModelBindingDescription()
	{
		VkVertexInputBindingDescription modelBindingDescription{ };
		modelBindingDescription.binding = 1u;
		modelBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_INSTANCE;
		modelBindingDescription.stride = sizeof mat4;
		return modelBindingDescription;
	}

	constexpr VkVertexInputAttributeDescription makePositionAttributeDescription()
	{
		VkVertexInputAttributeDescription positionAttributeDescription{ };
		positionAttributeDescription.binding = 0u;
		positionAttributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
		positionAttributeDescription.location = 0u;
		positionAttributeDescription.offset = 0u;
		return positionAttributeDescription;
	}

	array<VkVertexInputAttributeDescription, 4u> makeModelAttributeDescription();

	constexpr VkPipelineVertexInputStateCreateInfo makePipelineVertexInputStateCreateInfo(
		const array<VkVertexInputAttributeDescription, 5u>& attributeDescriptions,
		const array<VkVertexInputBindingDescription, 2u>& bindingDescriptions)
	{
		VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo{ };
		vertexInputStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		vertexInputStateCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
		vertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
		vertexInputStateCreateInfo.pVertexBindingDescriptions = bindingDescriptions.data();
		vertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescriptions.size());
		return vertexInputStateCreateInfo;
	}

	constexpr VkPipelineInputAssemblyStateCreateInfo makePipelineInputAssemblyStateCreateInfo()
	{
		VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo{ };
		inputAssemblyStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssemblyStateCreateInfo.primitiveRestartEnable = VK_FALSE;
		inputAssemblyStateCreateInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		return inputAssemblyStateCreateInfo;
	}

	constexpr VkViewport makeViewport(const Size2D& size)
	{
		VkViewport viewport{ };
		viewport.x = 0.f;
		viewport.y = 0.f;
		viewport.width = static_cast<float>(size.w);
		viewport.height = static_cast<float>(size.h);
		viewport.minDepth = 0.f;
		viewport.maxDepth = 1.f;
		return viewport;
	}

	constexpr VkRect2D makeScissor(const Size2D& size)
	{
		VkRect2D scissor{ };
		scissor.offset = { 0, 0 };
		scissor.extent = { static_cast<uint32_t>(size.w), static_cast<uint32_t>(size.h) };
		return scissor;
	}

	constexpr VkPipelineViewportStateCreateInfo makePipelineViewportStateCreateInfo(const VkRect2D& scissor, const VkViewport& viewport)
	{
		VkPipelineViewportStateCreateInfo viewportStateCreateInfo{ };
		viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportStateCreateInfo.pScissors = &scissor;
		viewportStateCreateInfo.scissorCount = 1u;
		viewportStateCreateInfo.pViewports = &viewport;
		viewportStateCreateInfo.viewportCount = 1u;
		return viewportStateCreateInfo;
	}

	constexpr VkPipelineRasterizationStateCreateInfo makePipelineRasterizationStateCreateInfo()
	{
		VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo{ };
		rasterizationStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizationStateCreateInfo.cullMode = VK_CULL_MODE_NONE;
		rasterizationStateCreateInfo.depthBiasClamp = 0.f;
		rasterizationStateCreateInfo.depthBiasConstantFactor = 0.f;
		rasterizationStateCreateInfo.depthBiasEnable = VK_FALSE;
		rasterizationStateCreateInfo.depthBiasSlopeFactor = 1.f;
		rasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
		rasterizationStateCreateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizationStateCreateInfo.lineWidth = 1.f;
		rasterizationStateCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizationStateCreateInfo.rasterizerDiscardEnable = VK_FALSE;
		return rasterizationStateCreateInfo;
	}

	constexpr VkPipelineMultisampleStateCreateInfo makePipelineMultisampleStateCreateInfo()
	{
		VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo{ };
		multisampleStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampleStateCreateInfo.alphaToCoverageEnable = VK_FALSE;
		multisampleStateCreateInfo.alphaToOneEnable = VK_FALSE;
		multisampleStateCreateInfo.minSampleShading = 1.f;
		multisampleStateCreateInfo.pSampleMask = nullptr;
		multisampleStateCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		multisampleStateCreateInfo.sampleShadingEnable = VK_FALSE;
		return multisampleStateCreateInfo;
	}

	constexpr VkPipelineColorBlendAttachmentState makePipelineColorBlendAttachmentState()
	{
		VkPipelineColorBlendAttachmentState colorBlendAttachmentState{ };
		colorBlendAttachmentState.blendEnable = VK_TRUE;
		colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
			VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachmentState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		colorBlendAttachmentState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
		colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;
		colorBlendAttachmentState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
		colorBlendAttachmentState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
		return colorBlendAttachmentState;
	}

	constexpr VkPipelineColorBlendStateCreateInfo makePipelineColorBlendStateCreateInfo(
		const VkPipelineColorBlendAttachmentState& colorBlendAttachmentState)
	{
		VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo{ };
		colorBlendStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlendStateCreateInfo.attachmentCount = 1u;
		colorBlendStateCreateInfo.pAttachments = &colorBlendAttachmentState;
		colorBlendStateCreateInfo.blendConstants[0u] = 0.f;
		colorBlendStateCreateInfo.blendConstants[1u] = 0.f;
		colorBlendStateCreateInfo.blendConstants[2u] = 0.f;
		colorBlendStateCreateInfo.blendConstants[3u] = 0.f;
		colorBlendStateCreateInfo.logicOp = VK_LOGIC_OP_CLEAR;
		colorBlendStateCreateInfo.logicOpEnable = VK_FALSE;
		return colorBlendStateCreateInfo;
	}

	constexpr VkPipelineDynamicStateCreateInfo makePipelineDynamicStateCreateInfo(const array<VkDynamicState, 2u>& dynamicStates)
	{
		VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo{ };
		dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();
		return dynamicStateCreateInfo;
	}

	constexpr VkDescriptorSetLayoutBinding makeDescriptorSetLayoutBinding()
	{
		VkDescriptorSetLayoutBinding descriptorSetLayoutBinding{ };
		descriptorSetLayoutBinding.binding = 0u;
		descriptorSetLayoutBinding.descriptorCount = 1u;
		descriptorSetLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		return descriptorSetLayoutBinding;
	}

	constexpr VkAttachmentDescription2KHR makeAttachmentDescription2KHR()
	{
		VkAttachmentDescription2KHR attachmentDescription{ };
		attachmentDescription.sType = VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
		attachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		attachmentDescription.format = VK_FORMAT_B8G8R8A8_SRGB;
		attachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
		attachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		attachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		return attachmentDescription;
	}

	constexpr VkAttachmentReference2KHR makeAttachmentReference2KHR()
	{
		VkAttachmentReference2KHR attachmentRef{ };
		attachmentRef.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
		attachmentRef.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		attachmentRef.attachment = 0u;
		attachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		return attachmentRef;
	}

	constexpr VkSubpassDescription2KHR makeSubpassDescription2KHR(const VkAttachmentReference2KHR& attachmentRef)
	{
		VkSubpassDescription2KHR subpassDescription{ };
		subpassDescription.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
		subpassDescription.colorAttachmentCount = 1u;
		subpassDescription.pColorAttachments = &attachmentRef;
		subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		return subpassDescription;
	}

	constexpr VkSubpassDependency2KHR makeSubpassDependency2KHR()
	{
		VkSubpassDependency2KHR subpassDependency{ };
		subpassDependency.sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR;
		subpassDependency.dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		subpassDependency.srcAccessMask = 0u;
		subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		subpassDependency.dstSubpass = 0u;
		return subpassDependency;
	}

	VKAPI_ATTR VkResult VKAPI_CALL createRenderPass2KHR(VkDevice, const VkRenderPassCreateInfo2*, const VkAllocationCallbacks*, VkRenderPass*);

	constexpr VkBufferCreateInfo makeBufferCreateInfo(const unsigned int size, const VkBufferUsageFlags usage)
	{
		VkBufferCreateInfo bufferCreateInfo{ };
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		bufferCreateInfo.size = size;
		bufferCreateInfo.usage = usage;
		return bufferCreateInfo;
	}

	VkPhysicalDeviceMemoryProperties2 getPhysicalDeviceMemoryProperties2(const VkPhysicalDevice physicalDevice);

	constexpr VkMemoryAllocateInfo makeMemoryAllocateInfo(const unsigned int size, const unsigned int memoryTypeIndex)
	{
		VkMemoryAllocateInfo memoryAllocateInfo{ };
		memoryAllocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memoryAllocateInfo.allocationSize = size;
		memoryAllocateInfo.memoryTypeIndex = memoryTypeIndex;
		return memoryAllocateInfo;
	}

	constexpr VkBindBufferMemoryInfo makeBindBufferMemoryInfo(const VkBuffer buffer, const VkDeviceMemory deviceMemory,
		const unsigned int offset)
	{
		VkBindBufferMemoryInfo bindBufferMemoryInfo{ };
		bindBufferMemoryInfo.sType = VK_STRUCTURE_TYPE_BIND_BUFFER_MEMORY_INFO;
		bindBufferMemoryInfo.buffer = buffer;
		bindBufferMemoryInfo.memory = deviceMemory;
		bindBufferMemoryInfo.memoryOffset = offset;
		return bindBufferMemoryInfo;
	}

	constexpr VkMappedMemoryRange makeMappedMemoryRange(const VkDeviceMemory deviceMemory, const unsigned int offset, const unsigned int size)
	{
		VkMappedMemoryRange mappedMemoryRange{ };
		mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
		mappedMemoryRange.memory = deviceMemory;
		mappedMemoryRange.offset = offset;
		mappedMemoryRange.size = size;
		return mappedMemoryRange;
	}

	constexpr VkCommandPoolCreateInfo makeStagingCommandPoolCreateInfo(const uint32_t graphicsQueueFamilyIndex)
	{
		VkCommandPoolCreateInfo commandPoolCreateInfo{ };
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = graphicsQueueFamilyIndex;
		return commandPoolCreateInfo;
	}

	constexpr VkCommandBufferAllocateInfo makeStagingCommandBufferAllocateInfo(const VkCommandPool stagingCommandPool)
	{
		VkCommandBufferAllocateInfo stagingCommandBufferAllocateInfo{ };
		stagingCommandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		stagingCommandBufferAllocateInfo.commandBufferCount = 1u;
		stagingCommandBufferAllocateInfo.commandPool = stagingCommandPool;
		stagingCommandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		return stagingCommandBufferAllocateInfo;
	}

	constexpr VkBufferCopy makeBufferCopyRegion(const unsigned int size)
	{
		VkBufferCopy bufferCopyRegion{ };
		bufferCopyRegion.srcOffset = 0u;
		bufferCopyRegion.dstOffset = 0u;
		bufferCopyRegion.size = size;
		return bufferCopyRegion;
	}

	constexpr VkSubmitInfo makeStagingSubmitInfo(const VkCommandBuffer& stagingCommandBuffer)
	{
		VkSubmitInfo stagingSubmitInfo{ };
		stagingSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		stagingSubmitInfo.commandBufferCount = 1u;
		stagingSubmitInfo.pCommandBuffers = &stagingCommandBuffer;
		return stagingSubmitInfo;
	}

	constexpr VkWriteDescriptorSet makeDescriptorSetWrite(const VkDescriptorSet descriptorSet,
		const VkDescriptorBufferInfo& descriptorBufferInfo)
	{
		VkWriteDescriptorSet descriptorSetWrite{ };
		descriptorSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorSetWrite.dstArrayElement = 0u;
		descriptorSetWrite.descriptorCount = 1u;
		descriptorSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSetWrite.dstBinding = 0u;
		descriptorSetWrite.dstSet = descriptorSet;
		descriptorSetWrite.pBufferInfo = &descriptorBufferInfo;
		return descriptorSetWrite;
	}
}

#endif