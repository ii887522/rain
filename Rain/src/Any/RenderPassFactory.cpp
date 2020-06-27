#include "RenderPassFactory.h"
#include <vulkan/vulkan.h>
#include <exception>
#include "../Functions/factory.h"

using std::exception;

namespace ii887522::Rain
{
	VkRenderPass RenderPassFactory::make(const VkDevice device) const
	{
		VkRenderPass renderPass;
		if (createRenderPass2KHR(device, &renderPassCreateInfo, nullptr, &renderPass) != VK_SUCCESS)
			throw exception{ "Failed to create render pass!" };
		return renderPass;
	}
}