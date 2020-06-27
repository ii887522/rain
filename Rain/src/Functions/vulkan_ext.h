#ifndef II887522_RAIN_VULKAN_EXT_H
#define II887522_RAIN_VULKAN_EXT_H

#include <vulkan/vulkan.h>
#include "../Any/App.h"
#include <vector>

using std::vector;

namespace ii887522::Rain
{
	VkPhysicalDevice getPhysicalDevice(const App&);
	vector<VkQueueFamilyProperties2> getQueueFamiliesProperties(const VkPhysicalDevice);
	unsigned int getGraphicsQueueFamilyIndex(const vector<VkQueueFamilyProperties2>&);
	unsigned int getPresentQueueFamilyIndex(const VkPhysicalDevice, const VkSurfaceKHR, const vector<VkQueueFamilyProperties2>&);
	VKAPI_ATTR VkResult VKAPI_CALL getPhysicalDeviceSurfaceCapabilities2KHR(VkInstance, VkPhysicalDevice,
		const VkPhysicalDeviceSurfaceInfo2KHR*, VkSurfaceCapabilities2KHR*);

	unsigned int getUniformBufferAlignment(const VkPhysicalDevice);
	unsigned int getMemoryTypeIndex(const VkPhysicalDevice, const VkMemoryPropertyFlags);

	VKAPI_ATTR void VKAPI_CALL cmdBeginRenderPass2KHR(VkDevice, VkCommandBuffer, const VkRenderPassBeginInfo*, const VkSubpassBeginInfo*);
	VKAPI_ATTR void VKAPI_CALL cmdEndRenderPass2KHR(VkDevice, VkCommandBuffer, const VkSubpassEndInfo*);
}

#endif