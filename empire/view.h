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

template<typename T, typename U = int>
struct Edge
{          
	Link<T, U>* link;
	Line line;
	EdgeStyle style{BlackWhiteEdgeStyle};
};

template<typename T, typename U = int>
struct Vertex
{
	Node<T, U>* node;
	Point center;
	std::vector<Edge<T, U>> edges;
	VertexStyle style{BlackWhiteVertexStyle};

	Edge<T, U>& get_edge(int to)
	{
		for (int i = 0; i < edges.size(); ++i)
		{
			if (edges[i].link->to->index == to)
			{
				return edges[i];
			}
		}
		return edges.front(); // FIXME - throw exception?
	}
};

template<typename T, typename U = int>
using Vertexes = std::vector<Vertex<T, U>>;

template<typename T, typename U = int>
bool is_inside(Point const& p, Vertex<T, U> const& vertex)
{
	auto const R2 = vertex.style.R2();
	auto const d = p - vertex.center;
	return d.x * d.x + d.y * d.y <= R2;
}

template<typename T, typename U = int>
struct GraphView
{
	Vertexes<T, U> vertexes;
	GraphStyle style;
	
	void reset_styles()
	{
		for (auto& v: vertexes)
		{
			v.style.border = style.vertex.border;
			v.style.background = style.vertex.background;
			v.style.label.color = style.vertex.label.color;
			for (auto& e: v.edges) e.style.color = style.edge.color;
		}
	}
	
	int locate_vertex(Point p)
	{
		for (auto const& v: vertexes)
		{
			if (is_inside(p, v)) return v.node->index;
		}
		return -1;
	}
};

template<typename T, typename U = int>
void PlaceVertexes(Graph<T, U> const& graph, Grid const& grid, GraphView<T, U>& view)
{
	view.vertexes.resize(graph.nodes.size());
	Point p;
	for (auto i = 0; i < grid.size(); ++i)
	{
		for (auto j = 0; j < grid[i].size(); ++j)
		{
			if (grid[i][j])
			{
				auto index = *grid[i][j];
				auto node = graph.nodes[index].get();
				auto& v = view.vertexes[index];
				v.node = node;
				v.center = p;
				v.style = view.style.vertex;
			}
			p.x += view.style.dx();
		}
		p.x = 0;
		p.y += view.style.dy();
	}
}

template<typename T, typename U = int>
void CreateEdges(GraphView<T, U>& view)
{
	auto find_vertex = [vs = std::as_const(view.vertexes)](Node<T, U> const * const n)
	{
		auto found = std::find_if(std::cbegin(vs), std::cend(vs),
			[n](auto const& v) { return v.node == n; });
		return found != std::end(vs) ? &*found : nullptr;
	};

	for (auto& v: view.vertexes)
	{
		std::vector<Edge<T, U>> edges;
		for (auto& l: v.node->links)
		{
			Line line{v.center, find_vertex(l.to)->center};
			v.edges.push_back(Edge<T, U>{&l, line, view.style.edge});
		}
	}
};

template<typename T, typename U = int>
GraphView<T, U> CreateView(Graph<T, U> const& graph, Grid const& grid,
	GraphStyle style = BlackWhiteGraphStyle)
{
	GraphView<T, U> view;
	view.style = std::move(style);
	PlaceVertexes(graph, grid, view);
	CreateEdges(view);
	return view;
}

template<typename T, typename U = int>
std::pair<std::string, Point> GetEdgeLabel(Edge<T, U> const& edge)
{
	auto const cost = std::to_string(edge.link->cost);
	return {cost, (edge.line / 4.0).e};
}

template<typename T, typename U = int>
void DrawEdge(bwgui::Application& app, Edge<T, U> const& edge)
{	
	auto [label, position] = GetEdgeLabel(edge);
	bwgui::SetBrushColor(app.renderer(), edge.style.label.color);
	bwgui::DrawText(app.renderer(), label, position, edge.style.label.size);

	bwgui::SetBrushColor(app.renderer(), edge.style.color);
	bwgui::DrawLine(app.renderer(), edge.line);
}

template<typename T, typename U = int>
void DrawVertex(bwgui::Application& app, Vertex<T, U> const& vertex)
{
	bwgui::SetBrushColor(app.renderer(), vertex.style.background);
	bwgui::FillCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.border);
	bwgui::DrawCircle(app.renderer(), Circle{vertex.center, vertex.style.radius});
	bwgui::SetBrushColor(app.renderer(), vertex.style.label.color);
	bwgui::DrawChar(app.renderer(), vertex.node->value, vertex.center, vertex.style.label.size);
}

template<typename T, typename U = int>
void DrawMark(bwgui::Application& app, Marker const& mark)
{
	if (mark.edge_style.marker.visible)
	{
		auto const center = (mark.line - (mark.vertex_style.radius + mark.edge_style.marker.size)).e;
		bwgui::FillCircle(app.renderer(), Circle{center, mark.edge_style.marker.size});
	}
}

template<typename T, typename U = int>
void DrawGraph(bwgui::Application& app, GraphView<T, U> const& view, Point const& p = {})
{
	for (auto const& v: view.vertexes)
	{
		for (auto const& e: v.edges)
		{
			auto const line = e.line + p;
			DrawEdge<T, U>(app, {e.link, line, e.style});
			DrawMark<T, U>(app, {line, v.style, e.style});
		}
	}
	for (auto const& v: view.vertexes)
	{
		DrawVertex<T, U>(app, {v.node, p + v.center, {}, v.style});
	}
}

} // namespace empire

#endif // _VIEW_H_
