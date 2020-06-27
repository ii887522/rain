#ifndef II887522_RAIN_SHADER_MODULE_FACTORY_H
#define II887522_RAIN_SHADER_MODULE_FACTORY_H

#include <vector>
#include <vulkan/vulkan.h>
#include <string>

using std::vector;
using std::string;

namespace ii887522::Rain
{
	class ShaderModuleFactory final
	{
		// remove copy semantics
		ShaderModuleFactory(const ShaderModuleFactory&) = delete;
		ShaderModuleFactory& operator=(const ShaderModuleFactory&) = delete;

		// remove move semantics
		ShaderModuleFactory(ShaderModuleFactory&&) = delete;
		ShaderModuleFactory& operator=(ShaderModuleFactory&&) = delete;

		const vector<char> shader;
		VkShaderModuleCreateInfo shaderModuleCreateInfo;

	public:
		explicit ShaderModuleFactory(const string& fileName);
		VkShaderModule make(const VkDevice) const;
	};
}

#endif