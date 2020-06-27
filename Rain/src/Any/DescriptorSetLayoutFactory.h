#ifndef II887522_RAIN_DESCRIPTOR_SET_LAYOUT_FACTORY_H
#define II887522_RAIN_DESCRIPTOR_SET_LAYOUT_FACTORY_H

#include <vulkan/vulkan.h>
#include "../Functions/factory.h"

namespace ii887522::Rain
{
	class DescriptorSetLayoutFactory final
	{
		// remove copy semantics
		DescriptorSetLayoutFactory(const DescriptorSetLayoutFactory&) = delete;
		DescriptorSetLayoutFactory& operator=(const DescriptorSetLayoutFactory&) = delete;

		// remove move semantics
		DescriptorSetLayoutFactory(DescriptorSetLayoutFactory&&) = delete;
		DescriptorSetLayoutFactory& operator=(DescriptorSetLayoutFactory&&) = delete;

		const VkDescriptorSetLayoutBinding descriptorSetLayoutBinding;
		VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo;

	public:
		explicit constexpr DescriptorSetLayoutFactory() : descriptorSetLayoutBinding{ makeDescriptorSetLayoutBinding() },
			descriptorSetLayoutCreateInfo{ }
		{
			descriptorSetLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			descriptorSetLayoutCreateInfo.bindingCount = 1u;
			descriptorSetLayoutCreateInfo.pBindings = &descriptorSetLayoutBinding;
		}

		VkDescriptorSetLayout make(const VkDevice) const;
	};
}

#endif