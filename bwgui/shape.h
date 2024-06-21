/**
 * BW Gui library
 *
 * Shapes
 */

#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <vector>
#include <cmath>

namespace bwgui
{
	template <typename T>
	struct Point
	{
		T x, y;

		operator SDL_Point() const
		{
			return SDL_Point{static_cast<int>(x), static_cast<int>(y)};
		}

#ifdef __EMSCRIPTEN__
		operator SDL_FPoint() const
		{
			return SDL_FPoint{static_cast<float>(x), static_cast<float>(y)};
		}
#endif
	};

	template <typename T>
	Point<T> operator+(Point<T> const &lhs, Point<T> const &rhs)
	{
		return {lhs.x + rhs.x, lhs.y + rhs.y};
	}

	template <typename T>
	Point<T> operator-(Point<T> const &lhs, Point<T> const &rhs)
	{
		return {lhs.x - rhs.x, lhs.y - rhs.y};
	}

	template <typename T>
	Point<T> operator*(Point<T> const &p, T m)
	{
		return {p.x * m, p.y * m};
	}

	template <typename T>
	Point<T> operator/(Point<T> const &p, T d)
	{
		return {p.x / d, p.y / d};
	}

	template <typename T>
	struct Size
	{
		T w;
		T h;
	};

	template <typename T>
	struct Line
	{
		Point<T> b;
		Point<T> e;
	};

	template <typename T>
	Line<T> operator+(Line<T> const &l, T s)
	{
		auto const d = l.e - l.b;
		auto const c = std::sqrt(d.x * d.x + d.y * d.y);
		auto const dx = s * (d.x / c);
		auto const dy = s * (d.y / c);
		return {l.b, {l.e.x + dx, l.e.y + dy}};
	}

	template <typename T>
	Line<T> operator-(Line<T> const &l, T s)
	{
		return l + (-s);
	}

	template <typename T>
	Line<T> operator+(Line<T> const &l, Point<T> const &p)
	{
		return {l.b + p, l.e + p};
	}

	template <typename T>
	Line<T> operator-(Line<T> const &l, Point<T> const &p)
	{
		return {l.b - p, l.e - p};
	}

	template <typename T>
	Line<T> operator*(Line<T> const &l, T m)
	{
		return {l.b, l.b + (l.e - l.b) * m};
	}

	template <typename T>
	Line<T> operator/(Line<T> const &l, T d)
	{
		return {l.b, l.b + (l.e - l.b) / d};
	}

	template <typename T>
	using PolyLine = std::vector<Line<T>>;

	template <typename T>
	struct Rectangle
	{
		Point<T> p;
		Size<T> s;

		operator SDL_Rect() const
		{
			return SDL_Rect{static_cast<int>(p.x),
							static_cast<int>(p.y),
							static_cast<int>(s.w),
							static_cast<int>(s.h)};
		}
	};

	template <typename T>
	struct Triangle
	{
		Point<T> a;
		Point<T> b;
		Point<T> c;
	};

	template <typename T>
	struct Circle
	{
		Point<T> c;
		T r;
	};

	template <typename T>
	struct Arc
	{
		Point<T> b;
		Point<T> e;
		T height;
	};

} // namespace bwgui

#endif // _SHAPE_H_
