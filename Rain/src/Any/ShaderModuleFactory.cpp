#include "ShaderModuleFactory.h"
#include <vulkan/vulkan.h>
#include <string>
#include "../Functions/io.h"
#include <exception>

using std::string;
using std::exception;

namespace ii887522::Rain
{
	ShaderModuleFactory::ShaderModuleFactory(const string& fileName) : shader{ readFile(fileName) }, shaderModuleCreateInfo{ }
	{
		shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shaderModuleCreateInfo.codeSize = shader.size();
		shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(shader.data());
	}

	VkShaderModule ShaderModuleFactory::make(const VkDevice device) const
	{
		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &shaderModuleCreateInfo, nullptr, &shaderModule) != VK_SUCCESS)
			throw exception{ "Failed to create shader module!" };
		return shaderModule;
	}
}