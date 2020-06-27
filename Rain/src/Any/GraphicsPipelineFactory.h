#ifndef II887522_RAIN_GRAPHICS_PIPELINE_FACTORY_H
#define II887522_RAIN_GRAPHICS_PIPELINE_FACTORY_H

#include <vulkan/vulkan.h>
#include "ShaderModuleFactory.h"
#include "../Functions/factory.h"
#include "../Struct/Size2D.h"

namespace ii887522::Rain
{
	class GraphicsPipelineFactory final
	{
		// remove copy semantics
		GraphicsPipelineFactory(const GraphicsPipelineFactory&) = delete;
		GraphicsPipelineFactory& operator=(const GraphicsPipelineFactory&) = delete;

		// remove move semantics
		GraphicsPipelineFactory(GraphicsPipelineFactory&&) = delete;
		GraphicsPipelineFactory& operator=(GraphicsPipelineFactory&&) = delete;

		const VkDevice device;
		const VkShaderModule vertexShaderModule;
		const VkShaderModule fragmentShaderModule;
		const VkPipelineShaderStageCreateInfo shaderStageCreateInfos[2u];
		const array<VkVertexInputAttributeDescription, 4u> modelAttributeDescription;
		const array<VkVertexInputBindingDescription, 2u> bindingDescriptions;
		const array<VkVertexInputAttributeDescription, 5u> attributeDescriptions;
		const VkPipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
		const VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
		const VkRect2D scissor;
		const VkViewport viewport;
		const VkPipelineViewportStateCreateInfo viewportStateCreateInfo;
		const VkPipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
		const VkPipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
		const VkPipelineColorBlendAttachmentState colorBlendAttachmentState;
		const VkPipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
		const array<VkDynamicState, 2u> dynamicStates;
		const VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo;
		VkGraphicsPipelineCreateInfo graphicsPipelineCreateInfo;

	public:
		explicit GraphicsPipelineFactory(const VkDevice, const Size2D, const VkPipelineLayout, const VkRenderPass);
		VkPipeline make() const;
		~GraphicsPipelineFactory();
	};
}

#endif