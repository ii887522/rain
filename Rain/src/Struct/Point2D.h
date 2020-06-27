#ifndef II887522_RAIN_POINT2D_H
#define II887522_RAIN_POINT2D_H

namespace ii887522::Rain
{
	struct Point2D final
	{
		// remove copy semantics
		Point2D(const Point2D&) = delete;
		Point2D& operator=(const Point2D&) = delete;

		// remove move semantics
		Point2D(Point2D&&) = delete;
		Point2D& operator=(Point2D&&) = delete;

		const float x;
		const float y;

		explicit constexpr Point2D(const float x = 0.f, const float y = 0.f) : x{ x }, y{ y } { }
	};
}

#endif