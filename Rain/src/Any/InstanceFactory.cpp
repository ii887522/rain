#include "InstanceFactory.h"
#include <vulkan/vulkan.h>
#include <exception>

using std::exception;

namespace ii887522::Rain
{
	VkInstance InstanceFactory::make() const
	{
		VkInstance instance;
		if (vkCreateInstance(&instanceCreateInfo, nullptr, &instance) != VK_SUCCESS) throw exception{ "Failed to create instance!" };
		return instance;
	}
}