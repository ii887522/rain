#ifndef II887522_RAIN_RAIN_H
#define II887522_RAIN_RAIN_H

#include <random>
#include "../Struct/Drop.h"
#include "constants.h"
#include "../Functions/control_flow.h"
#include "../Struct/Box.h"
#include "Enums.h"
#include "../Struct/Point2D.h"
#include "../Struct/Size2D.h"

using std::default_random_engine;

namespace ii887522::Rain
{
	// Not Thread Safe
	// See also ../Functions/main.cpp if you are looking for main entry point
	class Rain final
	{
		// remove copy semantics
		Rain(const Rain&) = delete;
		Rain& operator=(const Rain&) = delete;

		// remove move semantics
		Rain(Rain&&) = delete;
		Rain& operator=(Rain&&) = delete;

		const Box box;
		const float dropSize;
		default_random_engine randomEngine;
		Drop drops[dropCount];
		Size2D parentSize;
		float speed;

	public:
		explicit Rain();

		constexpr void reactWindowResize(const Size2D& newSize)
		{
			parentSize = newSize;
		}

		constexpr void reactMouseMove(const Point2D& mousePosition)
		{
			constexpr auto maxSpeed{ 16.f };
			speed = mousePosition.x / parentSize.w * maxSpeed;
		}

		void fall(const float dt);

		constexpr void serializeModels(void*const data)
		{
			loop(dropCount, [=](const auto i) {
				reinterpret_cast<decltype(Drop::model)*>(data)[i] = drops[i].model;
				return Action::CONTINUE;
			});
		}
	};
}

#endif