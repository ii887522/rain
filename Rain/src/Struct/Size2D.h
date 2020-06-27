#ifndef II887522_RAIN_SIZE2D_H
#define II887522_RAIN_SIZE2D_H

namespace ii887522::Rain
{
	struct Size2D final
	{
		// remove move semantics
		Size2D(Size2D&&) = delete;
		Size2D& operator=(Size2D&&) = delete;

		int w;
		int h;

		explicit constexpr Size2D(const int w = 0, const int h = 0) : w{ w }, h{ h } { }
		constexpr Size2D(const Size2D&) = default;
		constexpr Size2D& operator=(const Size2D&) = default;
	};
}

#endif