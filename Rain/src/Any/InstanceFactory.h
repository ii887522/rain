#ifndef II887522_RAIN_INSTANCE_FACTORY_H
#define II887522_RAIN_INSTANCE_FACTORY_H

#include <vulkan/vulkan.h>
#include "../Functions/factory.h"
#include "constants.h"

namespace ii887522::Rain
{
	class InstanceFactory final
	{
		// remove copy semantics
		InstanceFactory(const InstanceFactory&) = delete;
		InstanceFactory& operator=(const InstanceFactory&) = delete;

		// remove move semantics
		InstanceFactory(InstanceFactory&&) = delete;
		InstanceFactory& operator=(InstanceFactory&&) = delete;

		const VkApplicationInfo appInfo;
		const VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo;
		VkInstanceCreateInfo instanceCreateInfo;

	public:
		explicit constexpr InstanceFactory() : appInfo{ makeAppInfo() },
			debugCreateInfo{ isDebugging ? makeDebugCreateInfo() : VkDebugUtilsMessengerCreateInfoEXT{ } }, instanceCreateInfo{ }
		{
			instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			instanceCreateInfo.pApplicationInfo = &appInfo;
			instanceCreateInfo.enabledExtensionCount = sizeof instanceExtensions / sizeof(char*);
			instanceCreateInfo.ppEnabledExtensionNames = instanceExtensions;
			if constexpr (isDebugging)
			{
				instanceCreateInfo.enabledLayerCount = sizeof layers / sizeof(char*);
				instanceCreateInfo.ppEnabledLayerNames = layers;
				instanceCreateInfo.pNext = &debugCreateInfo;
			}
		}

		VkInstance make() const;
	};
}

#endif