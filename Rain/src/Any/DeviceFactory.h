#ifndef II887522_RAIN_DEVICE_FACTORY_H
#define II887522_RAIN_DEVICE_FACTORY_H

#include <vulkan/vulkan.h>
#include <vector>

using std::vector;

namespace ii887522::Rain
{
	class DeviceFactory final
	{
		// remove copy semantics
		DeviceFactory(const DeviceFactory&) = delete;
		DeviceFactory& operator=(const DeviceFactory&) = delete;

		// remove move semantics
		DeviceFactory(DeviceFactory&&) = delete;
		DeviceFactory& operator=(DeviceFactory&&) = delete;

		VkPhysicalDeviceFeatures2 physicalDeviceFeatures;
		const vector<VkDeviceQueueCreateInfo> deviceQueueCreateInfos;
		VkDeviceCreateInfo deviceCreateInfo;

	public:
		explicit DeviceFactory(const vector<VkDeviceQueueCreateInfo>&);
		VkDevice make(const VkPhysicalDevice) const;
	};
}

#endif