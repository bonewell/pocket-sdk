#include <cmath>
#include <stack>

#include "bwgui/application.h"

using Point = bwgui::Point<double>;
using Triangle = bwgui::Triangle<double>;

class SierpinskiGasket : public bwgui::Application
{
public:
	SierpinskiGasket(int level)
		: Application("sierpinski_gasket"),
		  level_{level} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	void Draw(int level, Triangle const &triangle);
	void DrawInLoop(int level, Triangle const &triangle);
	void DrawTriangle(Triangle const &triangle);
	std::vector<Triangle> SplitTriangle(Triangle const &triangle) const;

	int level_ = 0;
};

std::vector<Triangle> SierpinskiGasket::SplitTriangle(Triangle const &triangle) const
{
	Point p1{(triangle.b.x + triangle.a.x) / 2, (triangle.b.y + triangle.a.y) / 2};
	Point p2{(triangle.c.x + triangle.a.x) / 2, (triangle.c.y + triangle.a.y) / 2};
	Point p3{(triangle.c.x + triangle.b.x) / 2, triangle.b.y};
	return {
		{triangle.a, p1, p2},
		{p1, triangle.b, p3},
		{p2, p3, triangle.c}};
}

void SierpinskiGasket::DrawTriangle(Triangle const &triangle)
{
	bwgui::FillTriangle<double>(renderer(), triangle);
}

void SierpinskiGasket::Draw(int level, Triangle const &triangle)
{
	if (level == 0)
		DrawTriangle(triangle);
	else
	{
		auto const &triangles = SplitTriangle(triangle);
		for (auto const &t : triangles)
		{
			Draw(level - 1, t);
		}
	}
}

struct Info
{
	int level;
	Triangle shape;
};

void SierpinskiGasket::DrawInLoop(int level, Triangle const &triangle)
{
	std::stack<Info> info;
	info.push({level, triangle});
	while (!info.empty())
	{
		auto const e = info.top();
		info.pop();
		if (e.level == 0)
			DrawTriangle(e.shape);
		else
		{
			auto const &triangles = SplitTriangle(e.shape);
			for (auto const &t : triangles)
			{
				info.push({e.level - 1, t});
			}
		}
	}
}

void SierpinskiGasket::OnRender()
{
	bwgui::SetBrushColor(renderer(), bwgui::White);
	constexpr Point p1 = {1133.0, 30.0};
	constexpr Point p2 = {556.0, 1030.0};
	constexpr Point p3 = {1710.0, 1030.0};
	constexpr Triangle triangle = {p1, p2, p3};
	DrawInLoop(level_, triangle);
}

int main()
{
	SierpinskiGasket app{4};

	return app.Loop();
}
