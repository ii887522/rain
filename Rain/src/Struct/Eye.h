#ifndef II887522_RAIN_EYE_H
#define II887522_RAIN_EYE_H

#include <glm/glm.hpp>

using glm::mat4;

namespace ii887522::Rain
{
	struct Eye final
	{
		mat4 projection;
		mat4 view;

		explicit constexpr Eye(const mat4 projection, const mat4 view) : projection{ projection }, view{ view }
		{
			this->projection[1u][1u] *= -1.f;
		}
	};
}

#endif