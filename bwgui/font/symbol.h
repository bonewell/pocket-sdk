/**
 * BWGui library
 *
 * Font symbols
 */

#ifndef _SYMBOL_H_
#define _SYMBOL_H_

#include <array>
#include <bitset>

namespace bwgui::font
{
using Char = std::array<std::bitset<5>, 7>;

Char Space
{
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000,
	0b00000
};

Char Bang
{
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00100,
	0b00000,
	0b00100
};

Char LeftArrow
{
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b01000,
	0b00100,
	0b00010
};

Char RightArrow
{
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00010,
	0b00100,
	0b01000
};

} // namespace bwgui::font

#endif // _SYMBOL_H_
