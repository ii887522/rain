#ifndef II887522_RAIN_POINT3D_H
#define II887522_RAIN_POINT3D_H

namespace ii887522::Rain
{
	struct Point3D final
	{
		float x;
		float y;
		float z;

		explicit constexpr Point3D(const float x = 0.f, const float y = 0.f, const float z = 0.f) : x{ x }, y{ y }, z{ z } { }
	};
}

#endif