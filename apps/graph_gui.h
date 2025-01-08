#ifndef _GRAPH_GUI_H_
#define _GRAPH_GUI_H_

#include "../bwgui/action.h"
#include "../bwgui/application.h"

#include "../empire/graph.h"
#include "../empire/view.h"

template<typename T>
class GraphGui: public bwgui::Application
{
public:
	using graph_type = T;
	using node_type = typename graph_type::node_type;

	using graph_view = empire::GraphView<graph_type>;
	using vertex_type = typename graph_view::vertex_type;

	GraphGui(graph_type graph, empire::Grid const& grid,
		empire::GraphStyle style = empire::BlackWhiteGraphStyle, int delay = 300)
		: Application("graph_gui", delay),
		  graph_{std::move(graph)},
		  view_{empire::CreateView(graph_, grid, std::move(style))}{}

	void reset() { view_.reset_styles(); }
	vertex_type& vertex(node_type* node) { return view_.vertexes[node]; }
	graph_type& graph() { return graph_; }
	graph_view& view() { return view_; }

	virtual void OnTap(vertex_type& vertex) {};

private:
	void OnClick(bwgui::Point<int> point) override
	{
		empire::Point p{static_cast<double>(point.x), static_cast<double>(point.y)};
		auto v = view_.locate_vertex(p - position_);
		if (v) OnTap(*v);
	}

	void OnRender() override { empire::DrawGraph(*this, view_, position_); }

	graph_type graph_;
	graph_view view_;
	empire::Point position_{75, 75};
};

#endif // _GRAPH_GUI_H_
