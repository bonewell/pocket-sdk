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

#define LOG_ERROR() std::cerr << "[ERROR] " << __FILE__ << ':' << __LINE__ \
							  << " [" << __FUNCTION__ << "] " << SDL_GetError() << std::endl;

	enum class Rotation
	{
		CCW = -1,
		CW = 1
	};

	void SetBrushColor(SDL_Renderer *renderer, Color const &color)
	{
		if (SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a) != 0)
			LOG_ERROR();
	}

	Color GetBrushColor(SDL_Renderer *renderer)
	{
		Uint8 r, g, b, a;
		if (SDL_GetRenderDrawColor(renderer, &r, &g, &b, &a) != 0)
			LOG_ERROR();
		return Color{r, g, b, a};
	}

	template <typename T>
	void DrawPoint(SDL_Renderer *renderer, Point<T> const &point)
	{
		const SDL_Point p = point;
		if (SDL_RenderDrawPoint(renderer, p.x, p.y) != 0)
			LOG_ERROR();
	}

	template <typename T>
	void DrawLine(SDL_Renderer *renderer, Line<T> const &line)
	{
		const SDL_Point a = line.b;
		const SDL_Point b = line.e;
		if (SDL_RenderDrawLine(renderer, a.x, a.y, b.x, b.y) != 0)
			LOG_ERROR();
	}

	template <typename T>
	void DrawRectangle(SDL_Renderer *renderer, Rectangle<T> const &rectangle)
	{
		const SDL_Rect rect = rectangle;
		if (SDL_RenderDrawRect(renderer, &rect) != 0)
			LOG_ERROR();
	}

	template <typename T>
	void FillRectangle(SDL_Renderer *renderer, Rectangle<T> const &rectangle)
	{
		const SDL_Rect rect = rectangle;
		if (SDL_RenderFillRect(renderer, &rect) != 0)
			LOG_ERROR();
	}

	template <typename T>
	void DrawTriangle(SDL_Renderer *renderer, Triangle<T> const &triangle)
	{
		const std::array<SDL_Point, 4> points{triangle.a, triangle.b, triangle.c, triangle.a};
		if (SDL_RenderDrawLines(renderer, points.data(), points.size()) != 0)
			LOG_ERROR();
	}

	template <typename T>
	void FillTriangle(SDL_Renderer *renderer, Triangle<T> const &triangle)
	{
#ifdef __EMSCRIPTEN__
		auto color = GetBrushColor(renderer);
		const std::array<SDL_Vertex, 3> verts{
			SDL_Vertex{triangle.a, color, SDL_FPoint{0}},
			SDL_Vertex{triangle.b, color, SDL_FPoint{0}},
			SDL_Vertex{triangle.c, color, SDL_FPoint{0}}};
		if (SDL_RenderGeometry(renderer, nullptr, verts.data(), verts.size(), nullptr, 0) != 0)
			LOG_ERROR();
#else
		auto x1 = triangle.a.x;
		auto y1 = triangle.a.y;
		auto x2 = triangle.b.x;
		auto y2 = triangle.b.y;
		auto x3 = triangle.c.x;
		auto y3 = triangle.c.y;
		auto d1 = std::sqrt(((y2 - y1) * (y2 - y1)) + ((x2 - x1) * (x2 - x1)));
		auto d2 = std::sqrt(((y3 - y2) * (y3 - y2)) + ((x3 - x2) * (x3 - x2)));
		auto d3 = std::sqrt(((y1 - y3) * (y1 - y3)) + ((x1 - x3) * (x1 - x3)));
		if (((d1 < d2) || (d1 == d2)) && ((d1 < d2) || (d1 == d2)))
		{
			auto tx = x1;
			auto ty = y1;
			auto vx = (x2 - x1) / d1;
			auto vy = (y2 - y1) / d1;
			auto counter = 0;
			while (counter < d1)
			{
				DrawLine<double>(renderer, {{x3, y3}, {tx, ty}});
				tx = tx + vx;
				ty = ty + vy;
				counter = counter + 1;
			}
		}
		else if ((d2 < d3) || (d2 == d3))
		{
			auto tx = x2;
			auto ty = y2;
			auto vx = (x3 - x2) / d2;
			auto vy = (y3 - y2) / d2;
			auto counter = 0;
			while (counter < d2)
			{
				DrawLine<double>(renderer, {{x1, y1}, {tx, ty}});
				tx = tx + vx;
				ty = ty + vy;
				counter = counter + 1;
			}
		}
		else
		{
			auto tx = x3;
			auto ty = y3;
			auto vx = (x1 - x3) / d3;
			auto vy = (y1 - y3) / d3;
			auto counter = 0;
			while (counter < d3)
			{
				DrawLine<double>(renderer, {{x2, y2}, {tx, ty}});
				tx = tx + vx;
				ty = ty + vy;
				counter = counter + 1;
			}
		}
#endif
	}

	template <typename T>
	void DrawCircle(SDL_Renderer *renderer, Circle<T> const &circle)
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

	template <typename T>
	void FillCircle(SDL_Renderer *renderer, Circle<T> const &circle)
	{
		auto const R2 = circle.r * circle.r;
		auto x = static_cast<int>(circle.r);
		while (x--)
		{
			auto y = static_cast<T>(std::sqrt(R2 - x * x));

			DrawLine<T>(renderer, {{circle.c.x - x, circle.c.y - y},
								   {circle.c.x - x, circle.c.y + y}});

			DrawLine<T>(renderer, {{circle.c.x + x, circle.c.y - y},
								   {circle.c.x + x, circle.c.y + y}});
		}
	}

	template <typename T>
	void DrawChar(SDL_Renderer *renderer, font::Char const &calligraphy, Point<T> const &p, double size)
	{
		auto const border = size * 0.3;
		Rectangle<T> rect{p, {size - border, size - border}};
		for (auto const &row : calligraphy)
		{
			int n = row.size();
			while (n--)
			{
				if (row.test(n))
					FillRectangle<T>(renderer, rect);
				rect.p.x += size;
			}
			rect.p.x = p.x;
			rect.p.y += size;
		}
	}

	template <typename T>
	void DrawChar(SDL_Renderer *renderer, char letter, Point<T> const &center, double size)
	{
		auto const &calligraphy = font::GetCalligraphy(letter);
		Point<T> const p{
			center.x - size * double(calligraphy[0].size()) / 2,
			center.y - size * double(calligraphy.size()) / 2};
		DrawChar(renderer, calligraphy, p, size);
	}

	template <typename T>
	void DrawText(SDL_Renderer *renderer, std::string const &text, Point<T> const &position, double size)
	{
		auto p = position;
		for (auto c : text)
		{
			auto const &calligraphy = font::GetCalligraphy(c);
			DrawChar(renderer, calligraphy, p, size);
			p.x += calligraphy[0].size() * size + size;
		}
	}

	template <typename T>
	void DrawArc(SDL_Renderer *renderer, Arc<T> const &arc, Rotation rotation)
	{
		auto const d = arc.e - arc.b;
		auto const L = std::sqrt(d.x * d.x + d.y * d.y) / 2;
		auto const H = std::abs(arc.height);
		auto const R = (L * L + H * H) / (H * 2);

		auto const side = static_cast<int>(rotation);
		auto const sina = d.y / (2 * L) * side;
		auto const cosa = d.x / (2 * L) * side;
		Point<T> p{
			arc.b.x + d.x / 2 - (R - H) * sina,
			arc.b.y + d.y / 2 + (R - H) * cosa};

		auto const bz = arc.b - p;
		auto const ez = arc.e - p;

		double A = bz.y - ez.y;
		double B = ez.x - bz.x;
		double C = bz.x * ez.y - ez.x * bz.y;

		auto const R2 = R * R;
		for (int i = -R; i < R; ++i)
		{
			double x = i;
			double y = std::sqrt(R2 - x * x);

			auto bound = bz.x;
			if (B * side > 0 || B * side < 0)
				bound = (-A * x - C) / B;
			if (B * side > 0)
			{
				if (-y < bound)
					DrawPoint<T>(renderer, {p.x + x, p.y - y});
				if (y < bound)
					DrawPoint<T>(renderer, {p.x + x, p.y + y});
			}
			else if (B * side < 0)
			{
				if (-y > bound)
					DrawPoint<T>(renderer, {p.x + x, p.y - y});
				if (y > bound)
					DrawPoint<T>(renderer, {p.x + x, p.y + y});
			}
			else if ((A * side < 0 && x > bound) || (A * side > 0 && x < bound))
			{
				DrawPoint<T>(renderer, {p.x + x, p.y - y});
				DrawPoint<T>(renderer, {p.x + x, p.y + y});
			}

			std::swap(x, y);

			bound = bz.y;
			if (A * side > 0 || A * side < 0)
				bound = (-B * y - C) / A;

			if (A * side > 0)
			{
				if (-x < bound)
					DrawPoint<T>(renderer, {p.x - x, p.y + y});
				if (x < bound)
					DrawPoint<T>(renderer, {p.x + x, p.y + y});
			}
			else if (A * side < 0)
			{
				if (-x > bound)
					DrawPoint<T>(renderer, {p.x - x, p.y + y});
				if (x > bound)
					DrawPoint<T>(renderer, {p.x + x, p.y + y});
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
