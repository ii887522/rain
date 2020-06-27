#ifndef II887522_RAIN_SIZE3D_H
#define II887522_RAIN_SIZE3D_H

namespace ii887522::Rain
{
	struct Size3D final
	{
		// remove operator= copy semantics
		Size3D& operator=(const Size3D&) = delete;

		// remove move semantics
		Size3D(Size3D&&) = delete;
		Size3D& operator=(Size3D&&) = delete;

		const float w;
		const float h;
		const float l;

		explicit constexpr Size3D(const float w = 0.f, const float h = 0.f, const float l = 0.f) : w{ w }, h{ h }, l{ l } { }
		constexpr Size3D(const Size3D&) = default;
	};
}

#endif