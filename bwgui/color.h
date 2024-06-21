/**
 * BWGui library
 *
 * Colors
 */

#ifndef _COLOR_H_
#define _COLOR_H_

#include <SDL.h>
#include <ostream>
#include <iomanip>

namespace bwgui
{
	struct Color
	{
		int r, g, b, a;

		operator SDL_Color() const
		{
			return SDL_Color{
				static_cast<Uint8>(r),
				static_cast<Uint8>(g),
				static_cast<Uint8>(b),
				static_cast<Uint8>(a)};
		}
	};
	bool operator==(Color const &lhs, Color const &rhs)
	{
		return lhs.r == rhs.r && lhs.g == rhs.g && lhs.b == rhs.b && lhs.a == rhs.a;
	}
	bool operator!=(Color const &lhs, Color const &rhs)
	{
		return !(lhs == rhs);
	}

	std::ostream &operator<<(std::ostream &out, Color const &color)
	{
		out << '#' << std::hex;
		out << std::setw(2) << std::setfill('0') << color.r;
		out << std::setw(2) << std::setfill('0') << color.g;
		out << std::setw(2) << std::setfill('0') << color.b;
		return out;
	}

	constexpr Color Black{0, 0, 0, SDL_ALPHA_OPAQUE};
	constexpr Color Pebble{51, 51, 51, SDL_ALPHA_OPAQUE};
	constexpr Color DimGray{105, 105, 105, SDL_ALPHA_OPAQUE};
	constexpr Color Gray{128, 128, 128, SDL_ALPHA_OPAQUE};
	constexpr Color Silver{192, 192, 192, SDL_ALPHA_OPAQUE};
	constexpr Color White{255, 255, 255, SDL_ALPHA_OPAQUE};
	constexpr Color Red{255, 0, 0, SDL_ALPHA_OPAQUE};
	constexpr Color Crimson{220, 20, 60, SDL_ALPHA_OPAQUE};
	constexpr Color Lime{0, 255, 0, SDL_ALPHA_OPAQUE};
	constexpr Color DarkGreen{0, 100, 0, SDL_ALPHA_OPAQUE};
	constexpr Color Green{0, 128, 0, SDL_ALPHA_OPAQUE};
	constexpr Color GreenYellow{173, 255, 47, SDL_ALPHA_OPAQUE};
	constexpr Color LightGreen{144, 238, 144, SDL_ALPHA_OPAQUE};
	constexpr Color Blue{0, 0, 255, SDL_ALPHA_OPAQUE};
	constexpr Color DodgerBlue{30, 144, 255, SDL_ALPHA_OPAQUE};
	constexpr Color DeepSkyBlue{0, 191, 255, SDL_ALPHA_OPAQUE};
	constexpr Color LightBlue{173, 216, 230, SDL_ALPHA_OPAQUE};
	constexpr Color MediumSlateBlue{123, 104, 238, SDL_ALPHA_OPAQUE};
	constexpr Color BlueViolet{138, 43, 226, SDL_ALPHA_OPAQUE};
	constexpr Color Yellow{255, 255, 0, SDL_ALPHA_OPAQUE};
	constexpr Color Gold{255, 215, 0, SDL_ALPHA_OPAQUE};
	constexpr Color Orange{255, 165, 0, SDL_ALPHA_OPAQUE};

} // namespace bwgui

#endif // _COLOR_H_
