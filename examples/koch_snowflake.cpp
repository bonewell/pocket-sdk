#include <cmath>

#include "bwgui/application.h"

using Point = bwgui::Point<double>;

Point calc_point(Point const &p, double angle, double length)
{
	auto x = p.x + std::cos(angle) * length;
	auto y = p.y + std::sin(angle) * length;
	return {x, y};
}

class KochSnowflake : public bwgui::Application
{
public:
	KochSnowflake(int level, double angle)
		: Application("koch_snowflake"),
		  level_{level},
		  angle_{M_PI * angle / 180} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	Point Draw(int level, Point const &point, double angle, double length);

	int level_ = 0;
	double angle_ = 0.0;
};

Point KochSnowflake::Draw(int level, Point const &point, double angle, double length)
{
	if (level == 0)
	{
		auto end_point = calc_point(point, angle, length);
		int x1 = static_cast<int>(point.x);
		int y1 = static_cast<int>(point.y);
		int x2 = static_cast<int>(end_point.x);
		int y2 = static_cast<int>(end_point.y);
		bwgui::DrawLine<int>(renderer(), {{x1, y1}, {x2, y2}});
		return end_point;
	}
	else
	{
		auto p2 = Draw(level - 1, point, angle, length / 3);
		auto p3 = Draw(level - 1, p2, angle - angle_, length / 3);
		auto p4 = Draw(level - 1, p3, angle + angle_, length / 3);
		return Draw(level - 1, p4, angle, length / 3);
	}
}

void KochSnowflake::OnRender()
{
	bwgui::SetBrushColor(renderer(), bwgui::White);
	constexpr double length = 1000;
	constexpr Point p1 = {1133.0, 300.0};
	auto p2 = Draw(level_, p1, M_PI / 3, length);
	auto p3 = Draw(level_, p2, M_PI, length);
	Draw(level_, p3, -M_PI / 3, length);
}

int main()
{
	KochSnowflake app{3, 80};

	return app.Loop();
}
