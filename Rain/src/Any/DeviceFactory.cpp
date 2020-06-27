#include "DeviceFactory.h"
#include <vulkan/vulkan.h>
#include <vector>
#include <exception>
#include "constants.h"

using std::vector;
using std::exception;

namespace ii887522::Rain
{
	DeviceFactory::DeviceFactory(const vector<VkDeviceQueueCreateInfo>& deviceQueueCreateInfos) : physicalDeviceFeatures{ },
		deviceQueueCreateInfos{ deviceQueueCreateInfos }, deviceCreateInfo{ }
	{
		physicalDeviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures.features;
		deviceCreateInfo.pQueueCreateInfos = deviceQueueCreateInfos.data();
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(deviceQueueCreateInfos.size());
		deviceCreateInfo.enabledExtensionCount = sizeof deviceExtensions / sizeof(char*);
		deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions;
		if constexpr (isDebugging)
		{
			deviceCreateInfo.enabledLayerCount = sizeof layers / sizeof(char*);
			deviceCreateInfo.ppEnabledLayerNames = layers;
		}
	}

	VkDevice DeviceFactory::make(const VkPhysicalDevice physicalDevice) const
	{
		VkDevice device;
		if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &device) != VK_SUCCESS)
			throw exception{ "Failed to create logical device!" };
		return device;
	}
}