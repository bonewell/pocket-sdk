#ifndef _VIEW_H_
#define _VIEW_H_

#include "../bwgui/action.h"
#include "../bwgui/application.h"

#include <optional>

#include "graph.h"
#include "view_style.h"

namespace empire
{

using Point = bwgui::Point<double>;
using Line = bwgui::Line<double>;
using Circle = bwgui::Circle<double>;
using Triangle = bwgui::Triangle<double>;

using VertexId = std::optional<int>;
using Grid = std::vector<std::vector<VertexId>>;

struct Marker
{
	Line line;
	VertexStyle vertex_style{BlackWhiteVertexStyle};
	EdgeStyle edge_style{BlackWhiteEdgeStyle};
};

template<typename T>
struct Edge
{          
	T* link;
	Line line;
	EdgeStyle style{BlackWhiteEdgeStyle};
};

template<typename T>
struct Vertex
{
	using link_type = typename T::link_type;

	T* node;
	Point center;
	std::vector<Edge<link_type>> edges;
	VertexStyle style{BlackWhiteVertexStyle};

	Edge<link_type>& get_edge(T const* to)
	{
		for (auto& e: edges)
		{ if (e.link->to == to) return e; }
		throw "not found";
	}
};

template<typename T>
bool is_inside(Point const& p, Vertex<T> const& vertex)
{
	auto const R2 = vertex.style.R2();
	auto const d = p - vertex.center;
	return d.x * d.x + d.y * d.y <= R2;
}

template<typename T>
struct GraphView
{
	using node_type = typename T::node_type;
	using vertex_type = Vertex<node_type>;
	using Vertexes = std::unordered_map<node_type*, vertex_type>;

	Vertexes vertexes;
	GraphStyle style;
	
	void reset_styles()
	{
		for (auto& [n, v]: vertexes)
		{
			v.style.border = style.vertex.border;
			v.style.background = style.vertex.background;
			v.style.label.color = style.vertex.label.color;
			for (auto& e: v.edges) e.style.color = style.edge.color;
		}
	}
	
	vertex_type* locate_vertex(Point const& p)
	{
		for (auto& [n, v]: vertexes)
		{
			if (is_inside(p, v)) return &v;
		}
		return nullptr;
	}
};

template<typename T>
void PlaceVertexes(T const& graph, Grid const& grid, GraphView<T>& view)
{
	using node_type = typename T::node_type;

	view.vertexes.reserve(graph.size());
	Point p{0., 0.};
	for (auto i = 0; i < grid.size(); ++i)
	{
		for (auto j = 0; j < grid[i].size(); ++j)
		{
			if (grid[i][j])
			{
				auto index = *grid[i][j];
				auto node = graph[index];
				auto [v, ok] = view.vertexes.emplace(node, Vertex<node_type>{node, p});
				if (ok) v->second.style = view.style.vertex;
			}
			p.x += view.style.dx();
		}
		p.x = 0;
		p.y += view.style.dy();
	}
}

template<typename T>
void CreateEdges(GraphView<T>& view)
{
	using link_type = typename T::link_type;

	for (auto& [n, from]: view.vertexes)
	{
		std::vector<Edge<link_type>> edges;
		for (auto& l: n->links)
		{
			auto const& to = view.vertexes[l.to];
			Line line{from.center, to.center};
			from.edges.push_back(Edge<link_type>{&l, line, view.style.edge});
		}
	}
};

template<typename T>
GraphView<T> CreateView(T const& graph, Grid const& grid,
	GraphStyle style = BlackWhiteGraphStyle)
{
	GraphView<T> view;
	view.style = std::move(style);
	PlaceVertexes(graph, grid, view);
	CreateEdges(view);
	return view;
}

template<typename T>
std::pair<std::string, Point> GetEdgeLabel(Edge<T> const& edge)
{
	using cost_type = typename T::cost_type;

	std::string cost;
	if constexpr (std::is_arithmetic_v<cost_type>)
	{
		cost = std::to_string(edge.link->cost);
	}
	else
	{
		cost = edge.link->cost;
	}
	return {cost, (edge.line / 3.0).e};
}

template<typename T>
void DrawEdge(bwgui::Application& app, Edge<T> const& edge)
{
	using cost_type = typename T::cost_type;

	if constexpr (!std::is_void_v<cost_type>)
	{
		auto [label, position] = GetEdgeLabel(edge);
		bwgui::SetBrushColor(app.renderer(), edge.style.label.color);
		bwgui::DrawText(app.renderer(), label, position, edge.style.label.size);
	}

	bwgui::SetBrushColor(app.renderer(), edge.style.color);
	bwgui::DrawLine(app.renderer(), edge.line);
}

template<typename T>
void DrawVertex(bwgui::Application& app, Vertex<T> const& vertex)
{
	bwgui::SetBrushColor(app.renderer(), vertex.style.background);
	bwgui::FillCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.border);
	bwgui::DrawCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.label.color);
	bwgui::DrawChar(app.renderer(), vertex.node->value, vertex.center, vertex.style.label.size);
}

void DrawMark(bwgui::Application& app, Marker const& mark)
{
	if (mark.edge_style.marker.visible)
	{
		auto const center = (mark.line - (mark.vertex_style.radius + mark.edge_style.marker.size)).e;
		bwgui::FillCircle(app.renderer(), Circle{center, mark.edge_style.marker.size});
	}
}

template<typename T>
void DrawGraph(bwgui::Application& app, GraphView<T> const& view, Point const& p = {})
{
	using node_type = typename T::node_type;
	using link_type = typename T::link_type;

	for (auto const& [n, v]: view.vertexes)
	{
		for (auto const& e: v.edges)
		{
			auto const line = e.line + p;
			DrawEdge<link_type>(app, {e.link, line, e.style});
			DrawMark(app, {line, v.style, e.style});
		}
	}
	for (auto const& [n, v]: view.vertexes)
	{
		DrawVertex<node_type>(app, {v.node, p + v.center, {}, v.style});
	}
}

} // namespace empire

#endif // _VIEW_H_
