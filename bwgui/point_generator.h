/**
 * BWGui library
 *
 * Random generator of points
 */

#ifndef _POINT_GENERATOR_H_
#define _POINT_GENERATOR_H_

#include "shape.h"

#include <random>
#include <vector>

namespace bwgui
{

using Engine = std::default_random_engine;

template<typename T>
auto make_distribution(T min, T max)
{
	if constexpr (std::is_floating_point_v<T>)
	{
		return std::uniform_real_distribution<T>{min, max};
	}
	else
	{
		return std::uniform_int_distribution<T>{min, max};
	}
}

template<typename T>
Point<T> generate_point(Point<T> const& min, Point<T> const& max, Engine& engine)
{
	auto xgen = make_distribution<T>(min.x, max.x);
	auto ygen = make_distribution<T>(min.y, max.y);

	return {xgen(engine), ygen(engine)};
}

template<typename T>
class PointGenerator
{
public:
	PointGenerator(T min, T max)
		: min_{std::move(min)},
		  max_{std::move(max)}
	{}
	
	T point() const
	{
		return generate_point(min_, max_, engine_);
	}

private:
	static Engine engine_;
	T min_;
	T max_;
};
template<typename T>
Engine PointGenerator<T>::engine_ = Engine{static_cast<unsigned>(time(nullptr))};

template<typename T>
std::vector<Point<T>> generate_points(Point<T> const& min, Point<T> const& max, int size)
{
	std::vector<Point<T>> points;
	points.reserve(size);

	PointGenerator<Point<T>> generator{min, max};
	while (size--)
	{
		points.push_back(generator.point());
	}
	return points;
}

} // namespace bwgui

#endif // _POINT_GENERATOR_H_
