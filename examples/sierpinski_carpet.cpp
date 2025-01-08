#include <cmath>

#include "bwgui/application.h"

using Point = bwgui::Point<double>;
using Size = bwgui::Size<double>;
using Rectangle = bwgui::Rectangle<double>;

class SierpinskiCarpet : public bwgui::Application
{
public:
	SierpinskiCarpet(int level)
		: Application("sierpinski_carpet"),
		  level_{level} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	void Draw(int level, Rectangle const &rect);
	void DrawRect(Rectangle const &rect);
	std::vector<Rectangle> SplitRect(Rectangle const &rect) const;

	int level_ = 0;
};

std::vector<Rectangle> SierpinskiCarpet::SplitRect(Rectangle const &rect) const
{
	double dx = rect.s.w / 3;
	std::array<double, 3> xs;
	xs[0] = rect.p.x;
	xs[1] = xs[0] + dx;
	xs[2] = xs[1] + dx;

	double dy = rect.s.h / 3;
	std::array<double, 3> ys;
	ys[0] = rect.p.y;
	ys[1] = ys[0] + dy;
	ys[2] = ys[1] + dy;

	std::vector<Rectangle> rects;
	rects.reserve(8);
	for (auto i = 0; i < xs.size(); ++i)
	{
		for (auto j = 0; j < ys.size(); ++j)
		{
			if (i != 1 || j != 1)
			{
				rects.push_back({{xs[i], ys[j]}, {dx, dy}});
			}
		}
	}
	return rects;
}

void SierpinskiCarpet::DrawRect(Rectangle const &rect)
{
	auto x = static_cast<int>(rect.p.x);
	auto y = static_cast<int>(rect.p.y);
	auto w = static_cast<int>(rect.s.w);
	auto h = static_cast<int>(rect.s.h);
	bwgui::FillRectangle<int>(renderer(), {{x, y}, {w, h}});
}

void SierpinskiCarpet::Draw(int level, Rectangle const &rect)
{
	if (level == 0)
		DrawRect(rect);
	else
	{
		auto const &rects = SplitRect(rect);
		for (auto const &r : rects)
		{
			Draw(level - 1, r);
		}
	}
}

void SierpinskiCarpet::OnRender()
{
	bwgui::SetBrushColor(renderer(), bwgui::White);
	constexpr Point p = {10.0, 10.0};
	constexpr Size s = {972.0, 972.0};
	constexpr Rectangle rect = {p, s};
	Draw(level_, rect);
}

int main()
{
	SierpinskiCarpet app{4};

	return app.Loop();
}
