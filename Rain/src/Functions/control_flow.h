#ifndef II887522_RAIN_CONTROL_FLOW_H
#define II887522_RAIN_CONTROL_FLOW_H

#include <functional>
#include "../Any/Enums.h"

using std::function;

namespace ii887522::Rain
{
	constexpr void loop(const unsigned int times, const function<Action(const unsigned int)>& run)
	{
		for (auto i{ 0u }; i != times; ++i) if (run(i) == Action::BREAK) break;
	}
}

#endif