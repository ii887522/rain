#include "vulkan_ext.h"
#include <vulkan/vulkan.h>
#include "../Any/App.h"
#include <exception>
#include <vector>
#include "control_flow.h"
#include "factory.h"
#include "../Any/Enums.h"

using std::exception;
using std::vector;

namespace ii887522::Rain
{
	VkPhysicalDevice getPhysicalDevice(const App& app)
	{
		for (const auto physicalDevice : app.getPhysicalDevices())
		{
			VkPhysicalDeviceProperties2 properties{ };
			properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
			vkGetPhysicalDeviceProperties2(physicalDevice, &properties);
			if (properties.properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) return physicalDevice;
		}
		throw exception{ "Failed to find a physical device!" };
	}

	vector<VkQueueFamilyProperties2> getQueueFamiliesProperties(const VkPhysicalDevice physicalDevice)
	{
		uint32_t queueFamilyPropertiesCount;
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyPropertiesCount, nullptr);
		vector<VkQueueFamilyProperties2> queueFamiliesProperties(queueFamilyPropertiesCount);
		loop(queueFamilyPropertiesCount, [&queueFamiliesProperties](const auto i) {
			queueFamiliesProperties[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			return Action::CONTINUE;
		});
		vkGetPhysicalDeviceQueueFamilyProperties2(physicalDevice, &queueFamilyPropertiesCount, queueFamiliesProperties.data());
		return queueFamiliesProperties;
	}

	unsigned int getGraphicsQueueFamilyIndex(const vector<VkQueueFamilyProperties2>& queueFamiliesProperties)
	{
		auto graphicsQueueFamilyIndex{ UINT_MAX };
		loop(static_cast<unsigned int>(queueFamiliesProperties.size()), [&](const auto i) {
			if (queueFamiliesProperties[i].queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphicsQueueFamilyIndex = i;
				return Action::BREAK;
			}
			return Action::CONTINUE;
		});
		if (graphicsQueueFamilyIndex == UINT_MAX) throw exception{ "Failed to find a graphics queue family!" };
		return graphicsQueueFamilyIndex;
	}

	unsigned int getPresentQueueFamilyIndex(const VkPhysicalDevice physicalDevice, const VkSurfaceKHR surface,
		const vector<VkQueueFamilyProperties2>& queueFamiliesProperties)
	{
		auto presentQueueFamilyIndex{ UINT_MAX };
		loop(static_cast<unsigned int>(queueFamiliesProperties.size()), [&](const auto i) {
			VkBool32 supported;
			if (vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supported) != VK_SUCCESS)
				throw exception{ "Failed to check if a physical device support a particular surface!" };
			if (supported == VK_FALSE) return Action::CONTINUE;
			presentQueueFamilyIndex = i;
			return Action::BREAK;
		});
		if (presentQueueFamilyIndex == UINT_MAX) throw exception{ "Failed to find a present queue family!" };
		return presentQueueFamilyIndex;
	}

	VKAPI_ATTR VkResult VKAPI_CALL getPhysicalDeviceSurfaceCapabilities2KHR(VkInstance instance, VkPhysicalDevice physicalDevice,
		const VkPhysicalDeviceSurfaceInfo2KHR* pSurfaceInfo, VkSurfaceCapabilities2KHR* pSurfaceCapabilities)
	{
		return reinterpret_cast<PFN_vkGetPhysicalDeviceSurfaceCapabilities2KHR>(vkGetInstanceProcAddr(
			instance, "vkGetPhysicalDeviceSurfaceCapabilities2KHR"
		))(physicalDevice, pSurfaceInfo, pSurfaceCapabilities);
	}

	unsigned int getUniformBufferAlignment(const VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceProperties2 properties{ };
		properties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		vkGetPhysicalDeviceProperties2(physicalDevice, &properties);
		return static_cast<unsigned int>(properties.properties.limits.minUniformBufferOffsetAlignment);
	}

	unsigned int getMemoryTypeIndex(const VkPhysicalDevice physicalDevice, const VkMemoryPropertyFlags memoryProperties)
	{
		const auto physicalDeviceMemoryProperties{ getPhysicalDeviceMemoryProperties2(physicalDevice) };
		auto memoryTypeIndex{ UINT_MAX };
		loop(physicalDeviceMemoryProperties.memoryProperties.memoryTypeCount, [&, memoryProperties](const auto i) {
			if ((physicalDeviceMemoryProperties.memoryProperties.memoryTypes[i].propertyFlags & memoryProperties) != memoryProperties)
				return Action::CONTINUE;
			memoryTypeIndex = i;
			return Action::BREAK;
		});
		if (memoryTypeIndex == UINT_MAX) throw exception{ "Failed to find a suitable memory type index for this parameter!" };
		return memoryTypeIndex;
	}

	VKAPI_ATTR void VKAPI_CALL cmdBeginRenderPass2KHR(VkDevice device, VkCommandBuffer commandBuffer,
		const VkRenderPassBeginInfo* pRenderPassBegin, const VkSubpassBeginInfo* pSubpassBeginInfo)
	{
		reinterpret_cast<PFN_vkCmdBeginRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdBeginRenderPass2KHR"))(
			commandBuffer, pRenderPassBegin, pSubpassBeginInfo
		);
	}

	VKAPI_ATTR void VKAPI_CALL cmdEndRenderPass2KHR(VkDevice device, VkCommandBuffer commandBuffer, const VkSubpassEndInfo* pSubpassEndInfo)
	{
		reinterpret_cast<PFN_vkCmdEndRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCmdEndRenderPass2KHR"))(commandBuffer, pSubpassEndInfo);
	}
}