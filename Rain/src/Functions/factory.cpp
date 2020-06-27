#include "factory.h"
#include <vulkan/vulkan.h>
#include <vector>
#include "vulkan_ext.h"
#include <array>
#include "control_flow.h"
#include "../Any/Enums.h"

using std::vector;
using std::array;

namespace ii887522::Rain
{
	VKAPI_ATTR VkResult VKAPI_CALL createDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pMessenger)
	{
		return reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"))(
			instance, pCreateInfo, pAllocator, pMessenger
		);
	}

	VKAPI_ATTR void VKAPI_CALL destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT messenger,
		const VkAllocationCallbacks* pAllocator)
	{
		return reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT"))(
			instance, messenger, pAllocator
		);
	}

	vector<VkDeviceQueueCreateInfo> makeDeviceQueueCreateInfos(const float& priority, const uint32_t graphicsQueueFamilyIndex,
		const uint32_t presentQueueFamilyIndex)
	{
		vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		deviceQueueCreateInfos.reserve(2u);
		deviceQueueCreateInfos.push_back(makeDeviceQueueCreateInfo(priority, graphicsQueueFamilyIndex));
		if (graphicsQueueFamilyIndex != presentQueueFamilyIndex)
			deviceQueueCreateInfos.push_back(makeDeviceQueueCreateInfo(priority, presentQueueFamilyIndex));
		return deviceQueueCreateInfos;
	}

	array<VkVertexInputAttributeDescription, 4u> makeModelAttributeDescription()
	{
		array<VkVertexInputAttributeDescription, 4u> modelAttributeDescription;
		loop(static_cast<unsigned int>(modelAttributeDescription.size()), [&](const auto i) {
			modelAttributeDescription[i].binding = 1u;
			modelAttributeDescription[i].format = VK_FORMAT_R32G32B32A32_SFLOAT;
			modelAttributeDescription[i].location = 1 + i;
			modelAttributeDescription[i].offset = sizeof vec4 * i;
			return Action::CONTINUE;
		});
		return modelAttributeDescription;
	}

	VKAPI_ATTR VkResult VKAPI_CALL createRenderPass2KHR(VkDevice device, const VkRenderPassCreateInfo2* pCreateInfo,
		const VkAllocationCallbacks* pAllocator, VkRenderPass* pRenderPass)
	{
		return reinterpret_cast<PFN_vkCreateRenderPass2KHR>(vkGetDeviceProcAddr(device, "vkCreateRenderPass2KHR"))(
			device, pCreateInfo, pAllocator, pRenderPass
		);
	}

	VkPhysicalDeviceMemoryProperties2 getPhysicalDeviceMemoryProperties2(const VkPhysicalDevice physicalDevice)
	{
		VkPhysicalDeviceMemoryProperties2 physicalDeviceMemoryProperties{ };
		physicalDeviceMemoryProperties.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_MEMORY_PROPERTIES_2;
		vkGetPhysicalDeviceMemoryProperties2(physicalDevice, &physicalDeviceMemoryProperties);
		return physicalDeviceMemoryProperties;
	}
}