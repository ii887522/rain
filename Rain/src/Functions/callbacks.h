#ifndef II887522_RAIN_CALLBACK_H
#define II887522_RAIN_CALLBACK_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace ii887522::Rain
{
	void reactMouseMove(GLFWwindow*, double x, double y);
	void reactWindowResize(GLFWwindow*, int newWidth, int newHeight);
	VkBool32 VKAPI_CALL debugLog(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT*, void* pUserData);
}

#endif