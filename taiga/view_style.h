#ifndef _VIEW_STYLE_H_
#define _VIEW_STYLE_H_

#include "../bwgui/color.h"

namespace taiga
{

struct FontStyle
{
	bwgui::Color color;
	double size{0.0};
};
constexpr FontStyle BlackWhiteFontStyle{bwgui::White, 7.0};

struct VertexStyle
{
	FontStyle label;
	bwgui::Color background;
	bwgui::Color border;
	double radius{0.0};

	constexpr double D() const { return 2.0 * radius; }
	constexpr double R2() const { return radius * radius; }
};
constexpr VertexStyle BlackWhiteVertexStyle{BlackWhiteFontStyle, bwgui::Black, bwgui::White, 50.0};

struct Padding
{
	double horizontal{0.0};
	double vertical{0.0};
};
constexpr Padding DefaultPadding{50.0, 50.0};

struct TuftStyle
{
	double height{0.0};
	bwgui::Color color;
};
constexpr TuftStyle BlackWhiteTuftStyle{50.0, bwgui::Gray};

struct TreeStyle
{
	VertexStyle vertex;
	bwgui::Color edge_color;
	TuftStyle tuft;
	Padding padding;

	constexpr double dy() { return 2.0 * vertex.radius + padding.vertical; }
};
constexpr TreeStyle BlackWhiteTreeStyle{BlackWhiteVertexStyle, bwgui::White,
	BlackWhiteTuftStyle, DefaultPadding};

} // namespace taiga

#endif // _VIEW_STYLE_H_
