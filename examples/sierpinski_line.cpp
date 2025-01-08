#include <cmath>

#include "bwgui/application.h"

using Point = bwgui::Point<double>;

class SierpinskiLine : public bwgui::Application
{
public:
	SierpinskiLine(int level)
		: Application("sierpinski_line"),
		  level_{level} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	Point Draw(int level, Point const &point, double dx, double dy);
	Point DrawLine(Point const &point, double dx, double dy);
	Point SierpRight(int level, Point const &point, double dx, double dy);
	Point SierpDown(int level, Point const &point, double dx, double dy);
	Point SierpLeft(int level, Point const &point, double dx, double dy);
	Point SierpUp(int level, Point const &point, double dx, double dy);

	int level_ = 0;
};

Point SierpinskiLine::DrawLine(Point const &point, double dx, double dy)
{
	Point end_point{point.x + dx, point.y + dy};
	int x1 = static_cast<int>(point.x);
	int y1 = static_cast<int>(point.y);
	int x2 = static_cast<int>(end_point.x);
	int y2 = static_cast<int>(end_point.y);
	bwgui::DrawLine<int>(renderer(), {{x1, y1}, {x2, y2}});
	return end_point;
}

Point SierpinskiLine::SierpRight(int level, Point const &point, double dx, double dy)
{
	if (level > 0)
	{
		--level;
		Point p = point;
		p = SierpRight(level, p, dx, dy);
		p = DrawLine(p, dx, dy);
		p = SierpDown(level, p, dx, dy);
		p = DrawLine(p, 2 * dx, 0);
		p = SierpUp(level, p, dx, dy);
		p = DrawLine(p, dx, -dy);
		p = SierpRight(level, p, dx, dy);
		return p;
	}
	return point;
}

Point SierpinskiLine::SierpDown(int level, Point const &point, double dx, double dy)
{
	if (level > 0)
	{
		--level;
		Point p = point;
		p = SierpDown(level, p, dx, dy);
		p = DrawLine(p, -dx, dy);
		p = SierpLeft(level, p, dx, dy);
		p = DrawLine(p, 0, 2 * dy);
		p = SierpRight(level, p, dx, dy);
		p = DrawLine(p, dx, dy);
		p = SierpDown(level, p, dx, dy);
		return p;
	}
	return point;
}

Point SierpinskiLine::SierpLeft(int level, Point const &point, double dx, double dy)
{
	if (level > 0)
	{
		--level;
		Point p = point;
		p = SierpLeft(level, p, dx, dy);
		p = DrawLine(p, -dx, -dy);
		p = SierpUp(level, p, dx, dy);
		p = DrawLine(p, -2 * dx, 0);
		p = SierpDown(level, p, dx, dy);
		p = DrawLine(p, -dx, dy);
		p = SierpLeft(level, p, dx, dy);
		return p;
	}
	return point;
}

Point SierpinskiLine::SierpUp(int level, Point const &point, double dx, double dy)
{
	if (level > 0)
	{
		--level;
		Point p = point;
		p = SierpUp(level, p, dx, dy);
		p = DrawLine(p, dx, -dy);
		p = SierpRight(level, p, dx, dy);
		p = DrawLine(p, 0, -2 * dy);
		p = SierpLeft(level, p, dx, dy);
		p = DrawLine(p, -dx, -dy);
		p = SierpUp(level, p, dx, dy);
		return p;
	}
	return point;
}

Point SierpinskiLine::Draw(int level, Point const &point, double dx, double dy)
{
	Point p = point;
	p = SierpRight(level, p, dx, dy);
	p = DrawLine(p, dx, dy);
	p = SierpDown(level, p, dx, dy);
	p = DrawLine(p, -dx, dy);
	p = SierpLeft(level, p, dx, dy);
	p = DrawLine(p, -dx, -dy);
	p = SierpUp(level, p, dx, dy);
	p = DrawLine(p, dx, -dy);
	return p;
}

void SierpinskiLine::OnRender()
{
	bwgui::SetBrushColor(renderer(), bwgui::White);
	constexpr double length = 30;
	constexpr Point p1 = {100.0, 10.0};
	Draw(level_, p1, length, length);
}

int main()
{
	SierpinskiLine app{3};

	return app.Loop();
}
