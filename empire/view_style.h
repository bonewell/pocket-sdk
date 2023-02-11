#ifndef _VIEW_STYLE_H_
#define _VIEW_STYLE_H_

#include "../bwgui/color.h"

namespace empire
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
constexpr Padding DefaultPadding{200.0, 200.0};

struct EdgeStyle
{
	FontStyle label;
	bwgui::Color color;
};
constexpr EdgeStyle BlackWhiteEdgeStyle{{bwgui::White, 3.0}, bwgui::Gray};

struct GraphStyle
{
	VertexStyle vertex;
	EdgeStyle edge;
	Padding padding;

	constexpr double dx() { return 2.0 * vertex.radius + padding.horizontal; }
	constexpr double dy() { return 2.0 * vertex.radius + padding.vertical; }
};
constexpr GraphStyle BlackWhiteGraphStyle{BlackWhiteVertexStyle, BlackWhiteEdgeStyle, DefaultPadding};

} // namespace empire

#endif // _VIEW_STYLE_H_
