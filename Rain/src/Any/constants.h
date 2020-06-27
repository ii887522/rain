#ifndef II887522_RAIN_CONSTANTS_H
#define II887522_RAIN_CONSTANTS_H

#include <vulkan/vulkan.h>

namespace ii887522::Rain
{
#ifdef NDEBUG
	constexpr auto isDebugging{ false };
	const char*const layers[]{ nullptr };
	const char*const instanceExtensions[]{
		VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface", VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME
	};
#else
	constexpr auto isDebugging{ true };
	const char*const layers[]{ "VK_LAYER_KHRONOS_validation", "VK_LAYER_LUNARG_standard_validation" };
	const char*const instanceExtensions[]{
		VK_KHR_SURFACE_EXTENSION_NAME, "VK_KHR_win32_surface", VK_KHR_GET_SURFACE_CAPABILITIES_2_EXTENSION_NAME,
		VK_EXT_DEBUG_UTILS_EXTENSION_NAME
	};
#endif

	constexpr auto aspectRatio{ 1.777777791f };
	const char*const deviceExtensions[]{ VK_KHR_SWAPCHAIN_EXTENSION_NAME, VK_KHR_CREATE_RENDERPASS_2_EXTENSION_NAME };
	constexpr auto dropCount{ 24576u };
	constexpr auto maxFramesInFlight{ 8u };
}

#endif