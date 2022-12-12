/**
 * BW Gui library
 *
 * Actions
 */

#ifndef _ACTION_H_
#define _ACTION_H_

#include "color.h"
#include "font/font.h"
#include "shape.h"

#include <SDL.h>

#include <iostream>
#include <iomanip>

namespace bwgui
{
	
#define LOG_ERROR() std::cout << "[ERROR] " << __FILE__ << ':' << __LINE__ \
	<< " [" << __FUNCTION__ << "] " << SDL_GetError() << std::endl;

enum class Rotation { CCW = -1, CW = 1 };	

void SetBrushColor(SDL_Renderer* renderer, Color const& color)
{
	if (SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a) != 0) LOG_ERROR();
}

template<typename T>
void DrawPoint(SDL_Renderer* renderer, Point<T> const& point)
{
	auto x = static_cast<int>(point.x);
	auto y = static_cast<int>(point.y);
	if (SDL_RenderDrawPoint(renderer, x, y) != 0) LOG_ERROR();
}

template<typename T>
void DrawLine(SDL_Renderer* renderer, Line<T> const& line)
{
	auto x1 = static_cast<int>(line.b.x);
	auto y1 = static_cast<int>(line.b.y);
	auto x2 = static_cast<int>(line.e.x);
	auto y2 = static_cast<int>(line.e.y);
	if (SDL_RenderDrawLine(renderer, x1, y1, x2, y2) != 0) LOG_ERROR();
}

template<typename T>
void DrawRectangle(SDL_Renderer* renderer, Rectangle<T> const& rectangle)
{
	SDL_Rect rect
	{
		static_cast<int>(rectangle.p.x),
		static_cast<int>(rectangle.p.y),
		static_cast<int>(rectangle.s.w),
		static_cast<int>(rectangle.s.h)
	};
	if (SDL_RenderDrawRect(renderer, &rect) != 0) LOG_ERROR();
}

template<typename T>
void FillRectangle(SDL_Renderer* renderer, Rectangle<T> const& rectangle)
{
	SDL_Rect rect
	{
		static_cast<int>(rectangle.p.x),
		static_cast<int>(rectangle.p.y),
		static_cast<int>(rectangle.s.w),
		static_cast<int>(rectangle.s.h)
	};
	if (SDL_RenderFillRect(renderer, &rect) != 0) LOG_ERROR();
}

template<typename T>
void DrawCircle(SDL_Renderer* renderer, Circle<T> const& circle)
{
	auto const R2 = circle.r * circle.r;
	auto x = static_cast<int>(circle.r);
	while (x--)
	{
		auto y = static_cast<T>(std::sqrt(R2 - x * x));
		
		DrawPoint<T>(renderer, {circle.c.x - x, circle.c.y - y});
		DrawPoint<T>(renderer, {circle.c.x - x, circle.c.y + y});
		DrawPoint<T>(renderer, {circle.c.x + x, circle.c.y - y});
		DrawPoint<T>(renderer, {circle.c.x + x, circle.c.y + y});
		
		DrawPoint<T>(renderer, {circle.c.x - y, circle.c.y - x});
		DrawPoint<T>(renderer, {circle.c.x - y, circle.c.y + x});
		DrawPoint<T>(renderer, {circle.c.x + y, circle.c.y - x});
		DrawPoint<T>(renderer, {circle.c.x + y, circle.c.y + x});
	}
}

template<typename T>
void FillCircle(SDL_Renderer* renderer, Circle<T> const& circle)
{
	auto const R2 = circle.r * circle.r;
	auto x = static_cast<int>(circle.r);
	while (x--)
	{
		auto y = static_cast<T>(std::sqrt(R2 - x * x));

		DrawLine<T>(renderer, {
			{circle.c.x - x, circle.c.y - y},
			{circle.c.x - x, circle.c.y + y}
		});

		DrawLine<T>(renderer, {
			{circle.c.x + x, circle.c.y - y},
			{circle.c.x + x, circle.c.y + y}
		});
	}
}

template<typename T>
void DrawChar(SDL_Renderer* renderer, char letter, Point<T> const& center, double size)
{
	auto const& calligraphy = font::GetCalligraphy(letter);
	Point<T> const p
	{
		center.x - size * double(calligraphy[0].size()) / 2,
		center.y - size * double(calligraphy.size()) / 2
	};
	auto border = size * 0.3;
	Rectangle<T> rect{p, {size - border, size - border}};
	for (auto const& row: calligraphy)
	{
		int n = row.size();
		while (n--)
		{
			if (row.test(n)) FillRectangle<T>(renderer, rect);
			rect.p.x += size;
		}
		rect.p.x = p.x;
		rect.p.y += size;
	}
}

template<typename T>
void DrawArc(SDL_Renderer* renderer, Arc<T> const& arc, Rotation rotation)
{
	auto dx = arc.e.x - arc.b.x;
	auto dy = arc.e.y - arc.b.y;
	auto L = std::sqrt(dx * dx + dy * dy) / 2;
	auto H = std::abs(arc.height);
	auto R = (L * L + H * H) / (H * 2);

	auto side = static_cast<int>(rotation);
	auto sina = dy / (2 * L) * side;
	auto cosa = dx / (2 * L) * side;
	Point<T> p
	{
		arc.b.x + dx / 2 - (R - H) * sina,
		arc.b.y + dy / 2 + (R - H) * cosa
	};

	Point<T> bz{arc.b.x - p.x, arc.b.y - p.y};
	Point<T> ez{arc.e.x - p.x, arc.e.y - p.y};
	
	double A = bz.y - ez.y;
	double B = ez.x - bz.x;
	double C = bz.x * ez.y - ez.x * bz.y;

	auto const R2 = R * R;
	for (int i = -R; i < R; ++i)
	{
		double x = i;
		double y = std::sqrt(R2 - x * x);

		auto bound = bz.x;
		if (B * side > 0 || B * side < 0) bound = (-A * x - C) / B;
		if (B * side > 0)
		{
			if (-y < bound) DrawPoint<T>(renderer, {p.x + x, p.y - y});
			if (y < bound) DrawPoint<T>(renderer, {p.x + x, p.y + y});
		}
		else if (B * side < 0)
		{
			if (-y > bound) DrawPoint<T>(renderer, {p.x + x, p.y - y});
			if (y > bound) DrawPoint<T>(renderer, {p.x + x, p.y + y});
		}
		else if ((A * side < 0 && x > bound) || (A * side > 0 && x < bound))
		{		
			DrawPoint<T>(renderer, {p.x + x, p.y - y});
			DrawPoint<T>(renderer, {p.x + x, p.y + y});
		}

		std::swap(x, y);

		bound = bz.y;
		if (A * side > 0 || A * side < 0) bound = (-B * y - C) / A;

		if (A * side > 0)
		{
			if (-x < bound) DrawPoint<T>(renderer, {p.x - x, p.y + y});
			if (x < bound) DrawPoint<T>(renderer, {p.x + x, p.y + y});
		}
		else if (A * side < 0)
		{
			if (-x > bound) DrawPoint<T>(renderer, {p.x - x, p.y + y});
			if (x > bound) DrawPoint<T>(renderer, {p.x + x, p.y + y});
		}
		else if ((B * side < 0 && y > bound) || (B * side > 0 && y < bound))
		{
			DrawPoint<T>(renderer, {p.x - x, p.y + y});
			DrawPoint<T>(renderer, {p.x + x, p.y + y});	
		}
	}
}

} // namespace bwgui

#endif // _ACTION_H_
