#ifndef _VIEW_H_
#define _VIEW_H_

#include "../bwgui/action.h"
#include "../bwgui/application.h"
#include "../bwgui/shape.h"

#include "thread_tree.h"
#include "tree.h"
#include "view_style.h"

namespace taiga
{
using Point = bwgui::Point<double>;
using Size = bwgui::Size<double>;
using Line = bwgui::Line<double>;
using Rectangle = bwgui::Rectangle<double>;
using Circle = bwgui::Circle<double>;
using Arc = bwgui::Arc<double>;

struct Edge
{
	Line line;
	bwgui::Color const& color;
};

struct Tuft
{
	Point from;
	Point to;
	bwgui::Rotation rotation;
	TuftStyle const& style{BlackWhiteTuftStyle};
};

template<typename T, bool debug = false>
struct Vertex
{
	T value;
	Point center{};
	VertexStyle style{BlackWhiteVertexStyle};
};

struct DebugBox { Rectangle rect; bwgui::Color color; };

template<typename T>
struct Vertex<T, true> : Vertex<T, false> { DebugBox box; };

template<typename T>
bool operator<(Vertex<T> const& a, Vertex<T> const& b)
{
	return a.value < b.value;
}

template<typename T, bool debug = false>
struct View
{
	TreeStyle style;

	virtual void add_vertex(T&& value) = 0;
};

template<typename T>
struct View<T, true> : View<T, false>
{
	template<typename U>
	void add_box(Link<Vertex<T, true>, U>& link,
		Point const& p, Size const& s, bwgui::Color c)
	{
		if (link) link->value.box = {{p, s}, c};
	}
};

template<typename T, bool debug = false>
struct TreeView : View<T, debug>
{
	Link<Vertex<T, debug>> root;

	void add_vertex(T&& value) override
	{ add_node(root, {std::forward<T>(value), {}, this->style.vertex}); }
};

template<typename T, bool debug = false>
struct ThreadTreeView : View<T, debug>
{
	ThreadLink<Vertex<T, debug>> root;

	void add_vertex(T&& value) override
	{ add_thread_node(root, {std::forward<T>(value), {}, this->style.vertex}); }
};

void DrawEdge(bwgui::Application& app, Edge const& edge)
{
	bwgui::SetBrushColor(app.renderer(), edge.color);
	bwgui::DrawLine(app.renderer(), edge.line);
}

template<typename T, bool debug>
void DrawVertex(bwgui::Application& app, Vertex<T, debug> const& vertex)
{
	bwgui::SetBrushColor(app.renderer(), vertex.style.background);
	bwgui::FillCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.border);
	bwgui::DrawCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.label.color);
	bwgui::DrawChar(app.renderer(), vertex.value, vertex.center, vertex.style.label.size);
	if constexpr (debug)
	{
		bwgui::SetBrushColor(app.renderer(), vertex.box.color);
		bwgui::DrawRectangle(app.renderer(), vertex.box.rect);
	}
}

void DrawTuft(bwgui::Application& app, Tuft const& tuft)
{
	bwgui::SetBrushColor(app.renderer(), tuft.style.color);
	bwgui::DrawArc(app.renderer(), Arc{tuft.from, tuft.to, tuft.style.height}, tuft.rotation);
}

template<typename T>
using VertexFunctor = std::function<void(
	T const&,
	Link<T> const&,
	Link<T> const&)>;

template<typename T>
void traverse(Link<T> const& link, VertexFunctor<T> func)
{
	if (!link) return;
	func(link->value, link->left, link->right);
	if (link->left) traverse(link->left, func);
	if (link->right) traverse(link->right, func);
}

template<typename T, bool debug>
bool is_inside(Point const& p, Vertex<T, debug> const& vertex)
{
	auto const R2 = vertex.style.R2();
	auto const d = p - vertex.center;
	return d.x * d.x + d.y * d.y <= R2;
}

template<typename T, typename U = Thread>
Link<T, U>& locate(Link<T, U>& link, Point const& p)
{
	if (!link) return link;
	if (is_inside(p, link->value)) return link;
	if (p.x < link->value.center.x)
	{
		if (auto& l = locate(link->left, p)) return l;
		return locate(link->right, p);
	}
	else if (p.x >= link->value.center.x)
	{
		if (auto& l = locate(link->right, p)) return l;
		return locate(link->left, p);
	}
	return link;
}

template<typename T, bool debug>
void DrawTree(bwgui::Application& app, TreeView<T, debug> const& view, Point const& p = {})
{
	auto draw = [&app, &view, &p] (auto const& v, auto const& left, auto const& right)
	{
		if (left) DrawEdge(app, {{p + v.center, p + left->value.center}, view.style.edge_color});
		if (right) DrawEdge(app, {{p + v.center, p + right->value.center}, view.style.edge_color});
		DrawVertex<T, debug>(app, {v.value, p + v.center, v.style});
	};
	traverse<Vertex<T, debug>>(view.root, draw);
}

template<typename T>
using ThreadVertexFunctor = std::function<void(
	T const&,
	ThreadLink<T> const&,
	ThreadLink<T> const&,
	Threads<T> const&)>;

template<typename T>
void traverse(ThreadLink<T> const& link, ThreadVertexFunctor<T> func)
{
	if (!link) return;
	func(link->value, link->left, link->right, link->thread);
	if (link->left) traverse(link->left, func);
	if (link->right) traverse(link->right, func);
}

template<typename T, bool debug>
void DrawThreadTree(bwgui::Application& app, ThreadTreeView<T, debug> const& view, Point const& p = {})
{
	auto draw = [&app, &view, &p] (auto const& v, auto const& left, auto const& right, auto const& thread)
	{
		if (left) DrawEdge(app, {{p + v.center, p + left->value.center}, view.style.edge_color});
		if (right) DrawEdge(app, {{p + v.center, p + right->value.center}, view.style.edge_color});
		if (thread.left) DrawTuft(app, {p + v.center, p + thread.left->value.center, bwgui::Rotation::CW,
			{view.style.tuft.height, view.style.tuft.color}});
		if (thread.right) DrawTuft(app, {p + v.center, p + thread.right->value.center, bwgui::Rotation::CCW,
			{view.style.tuft.height, view.style.tuft.color}});
		DrawVertex<T, debug>(app, {v.value, p + v.center, v.style});
	};
	traverse<Vertex<T, debug>>(view.root, draw);
}

template<typename T>
using AddVertexFunctor = std::function<void(T&&)>;

template<typename T, typename U, typename V, bool debug>
double FillTree(
	Link<T, U> const& link,
	Link<Vertex<T, debug>, V>& vertex,
	View<T, debug>& view,
	Point const& p = {})
{
	if (!link) return 0.0;

	view.add_vertex(std::forward<T>(link->value)); 

	const Point left_point{p.x, p.y + view.style.dy()};
	auto left_width = FillTree<T, U, V, debug>(link->left, vertex->left, view, left_point);
	if constexpr (debug)
		view.add_box(vertex->left, left_point, {left_width, view.style.vertex.D()}, bwgui::Red);

	auto right_x = p.x + left_width + view.style.padding.horizontal;
	const Point right_point{right_x, p.y + view.style.dy()};
	auto right_width = FillTree<T, U, V, debug>(link->right, vertex->right, view, right_point);
	if constexpr (debug)
		view.add_box(vertex->right, right_point, {right_width, view.style.vertex.D()}, bwgui::Blue);

	double width{0.0};
	double parent_x{0.0};
	if (left_width > 0 && right_width > 0)
	{
		width = left_width + right_width + view.style.padding.horizontal;
		parent_x = p.x + left_width + 0.5 * view.style.padding.horizontal - view.style.vertex.radius;
	}
	else if (left_width > 0)
	{
		width = left_width + view.style.padding.horizontal;
		parent_x = p.x + width - view.style.vertex.D();
	}
	else if (right_width > 0)
	{
		width = right_width + view.style.padding.horizontal;
		parent_x = p.x;
	}
	else
	{
		width = view.style.vertex.D();
		parent_x = p.x;
	}

	vertex->value.center = {parent_x + view.style.vertex.radius, p.y + view.style.vertex.radius};

	return width;
}

template<typename T, bool debug = false>
TreeView<T, debug> CreateView(Link<T> const& tree, TreeStyle style = BlackWhiteTreeStyle)
{
	TreeView<T, debug> view;
	view.style = std::move(style);

	auto width = FillTree<T>(tree, view.root, view);
	if constexpr (debug) view.add_box(view.root, {}, {width, view.style.vertex.D()}, bwgui::Red);

	return view;
}

template<typename T, bool debug = false>
ThreadTreeView<T, debug> CreateView(ThreadLink<T> const& tree, TreeStyle style = BlackWhiteTreeStyle)
{
	ThreadTreeView<T, debug> view;
	view.style = std::move(style);

	auto width = FillTree<T>(tree, view.root, view);
	if constexpr (debug) view.add_box(view.root, {}, {width, view.style.vertex.D()}, bwgui::Red);

	return view;
}

} // namespace taiga

#endif // _VIEW_H_
