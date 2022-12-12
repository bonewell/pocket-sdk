#ifndef _QUAD_TREE_H_
#define _QUAD_TREE_H_

#include <memory>
#include <optional>

namespace taiga
{

template<typename T>
struct Point { T x; T y; };

template<typename T>
struct Size { T w; T h; };

template<typename T>
struct Area { Point<T> p; Size<T> s; };

template<typename T, typename I> struct QuadNode;

template<typename T, typename I>
using QuadLink = std::unique_ptr<QuadNode<T, I>>;

template<typename T, typename I>
using QuadTree = QuadLink<T, I>;

template<typename T, typename I>
struct QuadNode
{
	using Items = std::vector<I>;

	static constexpr int MaxItems = 5;

	QuadNode(Area<T> area) : area{std::move(area)}
	{
		mid = {
			area.p.x + area.s.w / 2,
			area.p.y + area.s.h / 2
		};
	}

	Area<T> area;
	Point<T> mid;

	std::optional<Items> items{{}};
	QuadLink<T, I> lt;
	QuadLink<T, I> lb;
	QuadLink<T, I> rt;
	QuadLink<T, I> rb;
};

template<typename T, typename I>
QuadLink<T, I> make_quad(Area<T> area)
{
	return std::make_unique<QuadNode<T, I>>(area);
}

template<typename T, typename I>
void add_item(QuadLink<T, I>& link, I&& v)
{
	if (!link) return;

	if (link->items && (link->items->size() + 1 > link->MaxItems))
	{
		Size<T> size{link->area.s.w / 2, link->area.s.h / 2};
		link->lt = make_quad<T, I>({link->area.p, size});
		link->lb = make_quad<T, I>({{link->area.p.x, link->mid.y}, size});
		link->rt = make_quad<T, I>({{link->mid.x, link->area.p.y}, size});
		link->rb = make_quad<T, I>({link->mid, size});
		
		for (auto& i: *link->items)
		{
			if (i.position().x < link->mid.x)
			{
				if (i.position().y < link->mid.y) add_item(link->lt, std::move(i));
				else add_item(link->lb, std::move(i));
			}
			else
			{
				if (i.position().y < link->mid.y) add_item(link->rt, std::move(i));
				else add_item(link->rb, std::move(i));
			}
		}
		link->items = std::nullopt;
	}

	if (link->items)
	{
		link->items->push_back(std::forward<I>(v));
	}
	else if (v.position().x < link->mid.x)
	{
		if (v.position().y < link->mid.y) add_item(link->lt, std::forward<I>(v));
		else add_item(link->lb, std::forward<I>(v));
	}
	else
	{
		if (v.position().y < link->mid.y) add_item(link->rt, std::forward<I>(v));
		else add_item(link->rb, std::forward<I>(v));
	}
}

template<typename T, typename I>
auto find_item(QuadLink<T, I> const& link, Point<T> const& point) -> decltype(&link->items->front())
{
	if (!link) return nullptr;
	if (!link->items)
	{
		if (point.x < link->mid.x)
		{
			if (point.y < link->mid.y) return find_item(link->lt, point);
			else return find_item(link->lb, point);
		}
		else
		{
			if (point.y < link->mid.y) return find_item(link->rt, point);
			else return find_item(link->rb, point);
		}
	}
	else
	{
		for (auto& i : *link->items)
		{
			
			if (i.inside(point)) return &i;
		}
	}
	return nullptr;
}

template<typename I>
using ItemVisitor = std::function<void(I const&)>;

template<typename T, typename I>
void traverse_items(QuadLink<T, I> const& link, ItemVisitor<I> func)
{
	if (!link) return;
	if (link->items)
	{
		for (auto const& i : *link->items) func(i);
	}
	traverse_items(link->lt, func);
	traverse_items(link->lb, func);
	traverse_items(link->rt, func);
	traverse_items(link->rb, func);
}

template<typename T, typename I>
using NodeVisitor = std::function<void(QuadNode<T, I> const&)>;

template<typename T, typename I>
void traverse_nodes(QuadLink<T, I> const& link, NodeVisitor<T, I> func)
{
	if (!link) return;
	func(*link);
	traverse_nodes(link->lt, func);
	traverse_nodes(link->lb, func);
	traverse_nodes(link->rt, func);
	traverse_nodes(link->rb, func);
}

} // namespace taiga

#endif // _QUAD_TREE_H_
