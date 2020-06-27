#define GLFW_INCLUDE_VULKAN

#include "callbacks.h"
#include "../Any/App.h"
#include <GLFW/glfw3.h>
#include "../Any/constants.h"
#include <iostream>
#include "../Struct/Point2D.h"

using std::cout;

namespace ii887522::Rain
{
	void reactMouseMove(GLFWwindow* window, double x, double y)
	{
		reinterpret_cast<App*>(glfwGetWindowUserPointer(window))->rain->reactMouseMove(Point2D{ static_cast<float>(x), static_cast<float>(y) });
	}

	void reactWindowResize(GLFWwindow* window, int newWidth, int newHeight)
	{
		auto app{ reinterpret_cast<App*>(glfwGetWindowUserPointer(window)) };
		if (newWidth != app->windowSize.w)
		{
			app->windowSize.w = newWidth;
			app->windowSize.h = static_cast<int>(newWidth / aspectRatio);
			glfwSetWindowSize(window, app->windowSize.w, app->windowSize.h);
		}
		else if (newHeight != app->windowSize.h)
		{
			app->windowSize.w = static_cast<int>(newHeight * aspectRatio);
			app->windowSize.h = newHeight;
			glfwSetWindowSize(window, app->windowSize.w, app->windowSize.h);
		}
		app->rain->reactWindowResize(Size2D{ app->windowSize.w, app->windowSize.h });
	}

	VkBool32 VKAPI_CALL debugLog(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		cout << pCallbackData->pMessage << '\n';
		return VK_FALSE;
	}
}