/**
 * BWGui library
 *
 * Font digits
 */

#ifndef _DIGIT_H_
#define _DIGIT_H_

#include <array>
#include <bitset>

namespace bwgui::font
{
using Char = std::array<std::bitset<5>, 7>;

Char D0
{
	0b01110,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b10001,
	0b01110
};

Char D1
{
	0b00100,
	0b11100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b11111
};

Char D2
{
	0b01110,
	0b10001,
	0b00001,
	0b00010,
	0b00100,
	0b01000,
	0b11111
};

Char D3
{
	0b01110,
	0b10001,
	0b00001,
	0b00110,
	0b00001,
	0b10001,
	0b01110
};

Char D4
{
	0b00001,
	0b00011,
	0b00101,
	0b01001,
	0b11111,
	0b00001,
	0b00001
};

Char D5
{
	0b11111,
	0b10000,
	0b10000,
	0b11110,
	0b00001,
	0b10001,
	0b01110
};

Char D6
{
	0b01110,
	0b10001,
	0b10000,
	0b11110,
	0b10001,
	0b10001,
	0b01110
};

Char D7
{
	0b11111,
	0b00001,
	0b00010,
	0b00100,
	0b01000,
	0b01000,
	0b01000
};

Char D8
{
	0b01110,
	0b10001,
	0b10001,
	0b01110,
	0b10001,
	0b10001,
	0b01110
};

Char D9
{
	0b01110,
	0b10001,
	0b10001,
	0b01111,
	0b00001,
	0b10001,
	0b01110
};

} // namespace bwgui::font

#endif // _DIGIT_H_
