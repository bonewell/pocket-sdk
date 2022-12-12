/**
 * BWGui library
 *
 * Font
 */

#ifndef _FONT_H_
#define _FONT_H_

#include "digit.h"
#include "upper.h"
#include "symbol.h"

#include <unordered_map>

namespace bwgui::font
{

Char const& GetCalligraphy(char letter)
{
	static std::unordered_map<char, Char> chars
	{
		{'0', D0}, {'1', D1}, {'2', D2}, {'3', D3}, {'4', D4},
		{'5', D5}, {'6', D6}, {'7', D7}, {'8', D8}, {'9', D9},
		{'A', A}, {'B', B}, {'C', C}, {'D', D}, {'E', E}, {'F', F}, {'G', G}, {'H', H}, {'I', I}, {'J', J},
		{'K', K}, {'L', L}, {'M', M}, {'N', N}, {'O', O}, {'P', P}, {'Q', Q}, {'R', R}, {'S', S}, {'T', T},
		{'U', U}, {'V', V}, {'W', W}, {'X', X}, {'Y', Y}, {'Z', Z},
		{' ', Space}, {'!', Bang}
	};
	return chars.at(letter);
}

} // namespace bwgui::font

#endif // _FONT_H_
