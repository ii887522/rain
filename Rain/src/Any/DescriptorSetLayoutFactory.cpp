#include "DescriptorSetLayoutFactory.h"
#include <vulkan/vulkan.h>
#include <exception>

using std::exception;

namespace ii887522::Rain
{
	VkDescriptorSetLayout DescriptorSetLayoutFactory::make(const VkDevice device) const
	{
		VkDescriptorSetLayout descriptorSetLayout;
		if (vkCreateDescriptorSetLayout(device, &descriptorSetLayoutCreateInfo, nullptr, &descriptorSetLayout) != VK_SUCCESS)
			throw exception{ "Failed to create descriptor set layout!" };
		return descriptorSetLayout;
	}
}