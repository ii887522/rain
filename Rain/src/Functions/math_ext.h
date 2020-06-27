#ifndef II887522_RAIN_MATH_EXT_H
#define II887522_RAIN_MATH_EXT_H

namespace ii887522::Rain
{
	constexpr unsigned int getAlignedSize(const unsigned int originalSize, const unsigned int alignment)
	{
		return static_cast<unsigned int>((originalSize / (alignment + 1u) + 1u) * alignment);
	}
}

#endif