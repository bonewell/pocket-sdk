#include <cmath>
#include <stack>

#include "bwgui/application.h"

using Point = bwgui::Point<double>;

class HilbertLine : public bwgui::Application
{
public:
	HilbertLine(int level)
		: Application("hilbert_line"),
		  level_{level} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	Point Draw(int level, Point const &point, double dx, double dy);
	void DrawInLoop(int level, Point const &point, double dx, double dy);
	Point DrawLine(Point const &point, double dx, double dy);

	int level_ = 0;
};

Point HilbertLine::DrawLine(Point const &point, double dx, double dy)
{
	Point end_point{point.x + dx, point.y + dy};
	bwgui::DrawLine<double>(renderer(), {point, end_point});
	return end_point;
}

Point HilbertLine::Draw(int level, Point const &point, double dx, double dy)
{
	Point p = point;
	if (level > 0)
		p = Draw(level - 1, p, dy, dx);
	p = DrawLine(p, dx, dy);
	if (level > 0)
		p = Draw(level - 1, p, dx, dy);
	p = DrawLine(p, dy, dx);
	if (level > 0)
		p = Draw(level - 1, p, dx, dy);
	p = DrawLine(p, -dx, -dy);
	if (level > 0)
		p = Draw(level - 1, p, -dy, -dx);
	return p;
}

struct HilbertInfo
{
	int level;
	double dx;
	double dy;
	bool draw{false};
};

void HilbertLine::DrawInLoop(int level, Point const &point, double dx, double dy)
{
	Point p = point;
	std::stack<HilbertInfo> info;
	info.push({level + 1, dx, dy});
	while (!info.empty())
	{
		auto const e = info.top();
		info.pop();
		if (e.draw)
		{
			p = DrawLine(p, e.dx, e.dy);
		}
		else if (e.level > 0)
		{
			info.push({e.level - 1, -e.dy, -e.dx});
			info.push({e.level - 1, -e.dx, -e.dy, true});
			info.push({e.level - 1, e.dx, e.dy});
			info.push({e.level - 1, e.dy, e.dx, true});
			info.push({e.level - 1, e.dx, e.dy});
			info.push({e.level - 1, e.dx, e.dy, true});
			info.push({e.level - 1, e.dy, e.dx});
		}
	}
}

void HilbertLine::OnRender()
{
	bwgui::SetBrushColor(renderer(), bwgui::White);
	constexpr double length = 100;
	constexpr Point p1 = {10.0, 10.0};
	DrawInLoop(level_, p1, length, 0);
	// Draw(level_, p1, length, 0);
}

int main()
{
	HilbertLine app{2};

	return app.Execute();
}
