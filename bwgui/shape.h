/**
 * BW Gui library
 * 
 * Shapes
 */

#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <vector>

namespace bwgui
{

template<typename T>
struct Point { T x; T y; };
template<typename T>
Point<T> operator+(Point<T> const& lhs, Point<T> const& rhs)
{ return {lhs.x + rhs.x, lhs.y + rhs.y}; }

template<typename T>
struct Size { T w; T h; };

template<typename T>
struct Line { Point<T> b; Point<T> e; };

template<typename T>
using PolyLine = std::vector<Line<T>>;

template<typename T>
struct Rectangle { Point<T> p; Size<T> s; };

template<typename T>
struct Triangle { Point<T> a; Point<T> b; Point<T> c; };

template<typename T>
struct Circle { Point<T> c; T r; };

template<typename T>
struct Arc { Point<T> b; Point<T> e; T height; };

} // namespace bwgui

#endif // _SHAPE_H_
