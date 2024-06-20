#include <cmath>

#include "core/application.h"

struct Point { double x{0.0}; double y{0.0}; };
struct Triangle { Point p1; Point p2; Point p3; };

class SierpinskiGasket: public Application
{
public:
	SierpinskiGasket(int level)
		: Application("sierpinski_gasket"),
		  level_{level} {}

	void OnLoop() override {}
	void OnRender() override;

private:
	void Draw(int level, Triangle const& triangle);
	void DrawInLoop(int level, Triangle const& triangle);
	void DrawTriangle(Triangle const& triangle);
	std::vector<Triangle> SplitTriangle(Triangle const& triangle) const;
	
	int level_ = 0;
};

std::vector<Triangle> SierpinskiGasket::SplitTriangle(Triangle const& triangle) const
{
	Point p1{(triangle.p2.x + triangle.p1.x) / 2, (triangle.p2.y + triangle.p1.y) / 2};
	Point p2{(triangle.p3.x + triangle.p1.x) / 2, (triangle.p3.y + triangle.p1.y) / 2};
	Point p3{(triangle.p3.x + triangle.p2.x) / 2, triangle.p2.y};
	return {
		{triangle.p1, p1, p2},
		{p1, triangle.p2, p3},
		{p2, p3, triangle.p3}
	};
}

void SierpinskiGasket::DrawTriangle(Triangle const& triangle)
{
	std::array<SDL_Point, 4> points;
	points[0] = {
		static_cast<int>(triangle.p1.x),
		static_cast<int>(triangle.p1.y)};
	points[1] = {
		static_cast<int>(triangle.p2.x),
		static_cast<int>(triangle.p2.y)};
	points[2] = {
		static_cast<int>(triangle.p3.x),
		static_cast<int>(triangle.p3.y)};
	points[3] = {
		static_cast<int>(triangle.p1.x),
		static_cast<int>(triangle.p1.y)};
	while (points[1].x <= points[2].x)
	{
		if (SDL_RenderDrawLines(renderer(), points.data(), points.size()) != 0) LOG_ERROR();
		points[1].x++;
		points[2].x--;
	}
}

void SierpinskiGasket::Draw(int level, Triangle const& triangle)
{
	if (level == 0) DrawTriangle(triangle);
	else
	{
		auto const& triangles = SplitTriangle(triangle);
		for (auto const& t: triangles)
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

void SierpinskiGasket::DrawInLoop(int level, Triangle const& triangle)
{
	std::stack<Info> info;
	info.push({level, triangle});
	while (!info.empty())
	{
		auto const e = info.top();
		info.pop();
		if (e.level == 0) DrawTriangle(e.shape);
		else
		{
			auto const& triangles = SplitTriangle(e.shape);
			for (auto const& t: triangles)
			{
				info.push({e.level - 1, t});
			}
		}
	}
}

void SierpinskiGasket::OnRender()
{
	if (SDL_SetRenderDrawColor(renderer(), 255, 255, 255, SDL_ALPHA_OPAQUE) != 0) LOG_ERROR();
	constexpr Point p1 = {1133.0, 30.0};
	constexpr Point p2 = {556.0, 1030.0};
	constexpr Point p3 = {1710.0, 1030.0};
	constexpr Triangle triangle = {p1, p2, p3};
	//Draw(level_, triangle);
	DrawInLoop(level_, triangle);
}

int main()
{
	SierpinskiGasket app{4};

	return app.Execute();
}
