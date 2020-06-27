#include "GraphicsPipelineFactory.h"
#include <vulkan/vulkan.h>
#include <exception>

using std::exception;

namespace ii887522::Rain
{
	GraphicsPipelineFactory::GraphicsPipelineFactory(const VkDevice device, const Size2D size, const VkPipelineLayout pipelineLayout,
		const VkRenderPass renderPass) : device{ device }, vertexShaderModule{ ShaderModuleFactory{ "res/vertex_shader.spv" }.make(device) },
		fragmentShaderModule{ ShaderModuleFactory{ "res/fragment_shader.spv" }.make(device) }, shaderStageCreateInfos{
			makePipelineShaderStageCreateInfo(vertexShaderModule, VK_SHADER_STAGE_VERTEX_BIT),
			makePipelineShaderStageCreateInfo(fragmentShaderModule, VK_SHADER_STAGE_FRAGMENT_BIT)
		}, modelAttributeDescription{ makeModelAttributeDescription() },
		bindingDescriptions{ makePositionBindingDescription(), makeModelBindingDescription() },
		attributeDescriptions{
			makePositionAttributeDescription(), modelAttributeDescription[0u], modelAttributeDescription[1u],
			modelAttributeDescription[2u], modelAttributeDescription[3u]
		}, vertexInputStateCreateInfo{ makePipelineVertexInputStateCreateInfo(attributeDescriptions, bindingDescriptions) },
		inputAssemblyStateCreateInfo{ makePipelineInputAssemblyStateCreateInfo() }, scissor{ makeScissor(size) },
		viewport{ makeViewport(size) }, viewportStateCreateInfo{ makePipelineViewportStateCreateInfo(scissor, viewport) },
		rasterizationStateCreateInfo{ makePipelineRasterizationStateCreateInfo() },
		multisampleStateCreateInfo{ makePipelineMultisampleStateCreateInfo() },
		colorBlendAttachmentState{ makePipelineColorBlendAttachmentState() },
		colorBlendStateCreateInfo{ makePipelineColorBlendStateCreateInfo(colorBlendAttachmentState) },
		dynamicStates{ VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR },
		dynamicStateCreateInfo{ makePipelineDynamicStateCreateInfo(dynamicStates) }, graphicsPipelineCreateInfo{ }
	{
		graphicsPipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		graphicsPipelineCreateInfo.layout = pipelineLayout;
		graphicsPipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
		graphicsPipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
		graphicsPipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
		graphicsPipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
		graphicsPipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
		graphicsPipelineCreateInfo.pStages = shaderStageCreateInfos;
		graphicsPipelineCreateInfo.stageCount = sizeof shaderStageCreateInfos / sizeof VkPipelineShaderStageCreateInfo;
		graphicsPipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
		graphicsPipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
		graphicsPipelineCreateInfo.renderPass = renderPass;
		graphicsPipelineCreateInfo.subpass = 0u;
		graphicsPipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
		graphicsPipelineCreateInfo.basePipelineIndex = -1;
	}

	VkPipeline GraphicsPipelineFactory::make() const
	{
		VkPipeline graphicsPipeline;
		if (vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1u, &graphicsPipelineCreateInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
			throw exception{ "Failed to create graphics pipeline!" };
		return graphicsPipeline;
	}

	GraphicsPipelineFactory::~GraphicsPipelineFactory()
	{
		vkDestroyShaderModule(device, vertexShaderModule, nullptr);
		vkDestroyShaderModule(device, fragmentShaderModule, nullptr);
	}
}