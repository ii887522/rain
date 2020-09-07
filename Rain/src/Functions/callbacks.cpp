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
		reinterpret_cast<App*>(glfwGetWindowUserPointer(window))->getRain()->reactMouseMove(Point2D{ static_cast<float>(x), static_cast<float>(y) });
	}

	void reactWindowResize(GLFWwindow* window, int newWidth, int newHeight)
	{
		auto app{ reinterpret_cast<App*>(glfwGetWindowUserPointer(window)) };
		if (newWidth != app->getWindowSize().w)
		{
			app->getWindowSize().w = newWidth;
			app->getWindowSize().h = static_cast<int>(newWidth / aspectRatio);
			glfwSetWindowSize(window, app->getWindowSize().w, app->getWindowSize().h);
		}
		else if (newHeight != app->getWindowSize().h)
		{
			app->getWindowSize().w = static_cast<int>(newHeight * aspectRatio);
			app->getWindowSize().h = newHeight;
			glfwSetWindowSize(window, app->getWindowSize().w, app->getWindowSize().h);
		}
		app->getRain()->reactWindowResize(Size2D{ app->getWindowSize().w, app->getWindowSize().h });
	}

	VkBool32 VKAPI_CALL debugLog(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void*)
	{
		cout << pCallbackData->pMessage << '\n';
		return VK_FALSE;
	}
}