#ifndef II887522_RAIN_APP_H
#define II887522_RAIN_APP_H

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>
#include "../Struct/Size2D.h"
#include <vector>
#include <array>
#include "constants.h"
#include "Enums.h"
#include "Rain.h"

using std::vector;
using std::array;

namespace ii887522::Rain
{
	// Not Thread Safe: it must only be used in main thread
	// See also ../Functions/main.cpp if you are looking for main entry point
	class App final
	{
		// remove copy semantics
		App(const App&) = delete;
		App& operator=(const App&) = delete;

		// remove move semantics
		App(App&&) = delete;
		App& operator=(App&&) = delete;

		GLFWwindow* window;
		GLFWimage ico;
		VkInstance instance;
		VkDebugUtilsMessengerEXT debugMessenger;
		VkSurfaceKHR surface;
		VkPhysicalDevice physicalDevice;
		unsigned int graphicsQueueFamilyIndex;
		unsigned int presentQueueFamilyIndex;
		VkDevice device;
		VkQueue graphicsQueue;
		VkQueue presentQueue;
		VkSwapchainKHR swapchain;
		vector<VkImage> swapchainImages;
		vector<VkImageView> swapchainImageViews;
		VkDescriptorSetLayout descriptorSetLayout;
		VkPipelineLayout pipelineLayout;
		VkRenderPass renderPass;
		vector<VkFramebuffer> swapchainFramebuffers;
		VkPipeline graphicsPipeline;
		VkDeviceMemory staticMemory;
		VkDeviceMemory dynamicMemory;
		unsigned int uniformBufferAlignment;
		unsigned int dynamicMemoryTypeIndex;
		VkBuffer vertexBuffer;
		VkBuffer indexBuffer;
		VkBuffer uniformBuffer;
		VkBuffer instanceBuffer;
		VkMemoryRequirements2 uniformBufferMemoryRequirements;
		VkMemoryRequirements2 instanceBufferMemoryRequirements;
		void* dynamicData;
		VkDescriptorPool descriptorPool;
		vector<VkDescriptorSet> descriptorSets;
		VkCommandPool commandPool;
		vector<VkCommandBuffer> commandBuffers;
		VkSemaphore imageAcquiredSemaphores[maxFramesInFlight];
		VkSemaphore renderDoneSemaphores[maxFramesInFlight];
		VkFence frameFences[maxFramesInFlight];
		unsigned int currentFrame;

		void configureWindow();
		void initVulkan();
		void initDebugMessenger();
		void initDevice();
		void initGraphicsQueue();
		void initPresentQueue();
		void initVulkanPart(const Event = Event::NONE);
		VkSwapchainKHR makeSwapchain() const;
		VkSwapchainCreateInfoKHR makeSwapchainCreateInfoKHR(const array<uint32_t, 2u>& queueFamilyIndices) const;
		VkSurfaceCapabilities2KHR getSurfaceCapabilities() const;
		void initSwapchainImages();
		void initSwapchainImageViews();
		VkImageView makeImageView(const VkImage) const;
		void initSwapchainFramebuffers();
		VkFramebuffer makeFramebuffer(const VkImageView) const;
		VkFramebufferCreateInfo makeFramebufferCreateInfo(const VkImageView&) const;
		void initPipelineLayout();

		constexpr VkPipelineLayoutCreateInfo makePipelineLayoutCreateInfo() const
		{
			VkPipelineLayoutCreateInfo layoutCreateInfo{ };
			layoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			layoutCreateInfo.pSetLayouts = &descriptorSetLayout;
			layoutCreateInfo.setLayoutCount = 1u;
			return layoutCreateInfo;
		}

		void initVertexBuffer(const unsigned int size);
		void initIndexBuffer(const unsigned int size);
		void initUniformBuffer(const unsigned int size);
		void initInstanceBuffer(const unsigned int size);
		VkMemoryRequirements2 getBufferMemoryRequirements2(const VkBuffer) const;
		VkDeviceMemory allocateDeviceMemory(const unsigned int size, const unsigned int memoryTypeIndex) const;
		void initDeviceMemory();
		VkBuffer makeStagingBuffer(const unsigned int size);
		VkCommandPool makeStagingCommandPool() const;
		VkCommandBuffer makeStagingCommandBuffer(const VkCommandPool stagingCommandPool) const;
		void initStagingMemory(const VkDeviceMemory stagingMemory, const VkMemoryRequirements2 left, const VkMemoryRequirements2 right) const;
		void copyStagingMemoryToDeviceMemory(const VkBuffer stagingVertexBuffer, const VkBuffer stagingIndexBuffer) const;
		void beginImmediateStagingCommandBufferSync(const VkCommandBuffer) const;
		void endImmediateStagingCommandBufferSync(const VkCommandBuffer) const;
		void initDescriptorPool();
		VkDescriptorPoolCreateInfo makeDescriptorPoolCreateInfo(const VkDescriptorPoolSize&) const;
		VkDescriptorPoolSize makeDescriptorPoolSize() const;
		void initDescriptorSets();
		VkDescriptorSetAllocateInfo makeDescriptorSetAllocateInfo(const vector<VkDescriptorSetLayout>&) const;
		void updateDescriptorSets();
		vector<VkDescriptorBufferInfo> makeDescriptorBufferInfos() const;
		vector<VkWriteDescriptorSet> makeDescriptorSetWrites(const vector<VkDescriptorBufferInfo>&) const;
		VkDescriptorBufferInfo makeDescriptorBufferInfo(const unsigned int i) const;
		void initCommandPool();
		void initCommandBuffers();
		VkCommandBufferAllocateInfo makeCommandBufferAllocateInfo() const;
		void recordCommandBuffers() const;
		void recordCommandBuffer(const unsigned int i) const;
		void beginRenderPass(const unsigned int i) const;
		void endRenderPass(const unsigned int i) const;
		VkRenderPassBeginInfo makeRenderPassBeginInfo(const VkClearValue& clearColor, const VkFramebuffer swapchainFramebuffer) const;
		void initImageAcquiredSemaphores();
		void initRenderDoneSemaphores();
		void initFrameFences();
		VkSemaphore makeSemaphore() const;
		void run(const float dt);
		VkFence makeFence() const;
		void syncFence(const VkFence) const;

		constexpr VkAcquireNextImageInfoKHR makeAcquireNextImageInfo() const
		{
			VkAcquireNextImageInfoKHR acquireNextImageInfo{ };
			acquireNextImageInfo.sType = VK_STRUCTURE_TYPE_ACQUIRE_NEXT_IMAGE_INFO_KHR;
			acquireNextImageInfo.deviceMask = 1u;
			acquireNextImageInfo.fence = VK_NULL_HANDLE;
			acquireNextImageInfo.semaphore = imageAcquiredSemaphores[currentFrame];
			acquireNextImageInfo.swapchain = swapchain;
			acquireNextImageInfo.timeout = UINT64_MAX;
			return acquireNextImageInfo;
		}

		// Return: acquired image index
		uint32_t acquireImageAsync();

		// Param imageIndex: the acquired image index returned from acquireImageAsync()
		void step(const uint32_t imageIndex, const float dt);

		// Param imageIndex: the acquired image index returned from acquireImageAsync()
		VkSubmitInfo makeSubmitInfo(const uint32_t imageIndex, const VkPipelineStageFlags& waitStage) const;

		// Param imageIndex: the acquired image index returned from acquireImageAsync()
		void renderAsync(const uint32_t imageIndex) const;

		// Param imageIndex: the acquired image index returned from acquireImageAsync()
		constexpr VkPresentInfoKHR makePresentInfo(const uint32_t& imageIndex) const
		{
			VkPresentInfoKHR presentInfo{ };
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.pImageIndices = &imageIndex;
			presentInfo.pSwapchains = &swapchain;
			presentInfo.swapchainCount = 1u;
			presentInfo.pWaitSemaphores = &renderDoneSemaphores[currentFrame];
			presentInfo.waitSemaphoreCount = 1u;
			return presentInfo;
		}

		// Param imageIndex: the acquired image index returned from acquireImageAsync()
		void releaseImageAsync(const uint32_t imageIndex);
		
		void destroyFrameFences() const;
		void destroyRenderDoneSemaphores() const;
		void destroyImageAcquiredSemaphores() const;
		void destroyVulkanPart(const Event = Event::NONE);
		void destroySwapchainFramebuffers() const;
		void destroySwapchainImageViews() const;

	public:
		Size2D windowSize;
		Rain*const rain;

		explicit App(const int maxHeight = 900);
		vector<VkPhysicalDevice> getPhysicalDevices() const;
		void eventLoop();
		~App();
	};
}

#endif