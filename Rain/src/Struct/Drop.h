#ifndef II887522_RAIN_DROP_H
#define II887522_RAIN_DROP_H

#include <array>
#include <glm/glm.hpp>
#include "Point3D.h"

using std::array;
using glm::vec2;
using glm::mat4;

namespace ii887522::Rain
{
	struct Drop final
	{
		// remove copy semantics
		Drop(const Drop&) = delete;
		Drop& operator=(const Drop&) = delete;

		// remove move semantics
		Drop(Drop&&) = delete;
		Drop& operator=(Drop&&) = delete;

		const array<vec2, 4u> positions;
		const array<unsigned int, 6u> indices;
		mat4 model;
		Point3D position;

		explicit constexpr Drop() : positions{
				vec2{ -.16667f, 1.f }, vec2{ .16667f, 1.f }, vec2{ .16667f, -1.f }, vec2{ -.16667f, -1.f }
			}, indices{ 0u, 1u, 2u, 2u, 3u, 0u }, model{ }, position{ } { }
	};
}

#endif