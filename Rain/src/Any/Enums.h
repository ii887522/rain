#ifndef II887522_RAIN_ENUMS_H
#define II887522_RAIN_ENUMS_H

namespace ii887522::Rain
{
	enum class Action : unsigned int
	{
		CONTINUE, BREAK
	};

	enum class Event : unsigned int
	{
		NONE, CREATE_SWAPCHAIN, SWAPCHAIN_RECREATED
	};
}

#endif