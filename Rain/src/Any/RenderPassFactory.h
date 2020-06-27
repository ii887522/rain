#ifndef II887522_RAIN_RENDER_PASS_FACTORY_H
#define II887522_RAIN_RENDER_PASS_FACTORY_H

#include <vulkan/vulkan.h>
#include "../Functions/factory.h"

namespace ii887522::Rain
{
	class RenderPassFactory final
	{
		// remove copy semantics
		RenderPassFactory(const RenderPassFactory&) = delete;
		RenderPassFactory& operator=(const RenderPassFactory&) = delete;

		// remove move semantics
		RenderPassFactory(RenderPassFactory&&) = delete;
		RenderPassFactory& operator=(RenderPassFactory&&) = delete;

		const VkAttachmentDescription2KHR attachmentDescription;
		const VkAttachmentReference2KHR attachmentRef;
		const VkSubpassDescription2KHR subpassDescription;
		const VkSubpassDependency2KHR subpassDependency;
		VkRenderPassCreateInfo2KHR renderPassCreateInfo;

	public:
		explicit constexpr RenderPassFactory() : attachmentDescription{ makeAttachmentDescription2KHR() },
			attachmentRef{ makeAttachmentReference2KHR() }, subpassDescription{ makeSubpassDescription2KHR(attachmentRef) },
			subpassDependency{ makeSubpassDependency2KHR() }, renderPassCreateInfo{ }
		{
			renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
			renderPassCreateInfo.attachmentCount = 1u;
			renderPassCreateInfo.pAttachments = &attachmentDescription;
			renderPassCreateInfo.dependencyCount = 1u;
			renderPassCreateInfo.pDependencies = &subpassDependency;
			renderPassCreateInfo.subpassCount = 1u;
			renderPassCreateInfo.pSubpasses = &subpassDescription;
		}

		VkRenderPass make(const VkDevice) const;
	};
}

#endif