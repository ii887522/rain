#ifndef II887522_RAIN_BOX_H
#define II887522_RAIN_BOX_H

#include "Point3D.h"
#include "Size3D.h"

namespace ii887522::Rain
{
	struct Box final
	{
		// remove copy semantics
		Box(const Box&) = delete;
		Box& operator=(const Box&) = delete;

		// remove move semantics
		Box(Box&&) = delete;
		Box& operator=(Box&&) = delete;

		const Point3D position;
		const Size3D size;

		explicit constexpr Box(const Point3D& position = Point3D{ }, const Size3D& size = Size3D{ }) : position{ position }, size{ size } { }
	};
}

#endif