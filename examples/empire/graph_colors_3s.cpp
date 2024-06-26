﻿#include "bwgui/action.h"
#include "bwgui/application.h"

#include "empire/colorizer.h"
#include "empire/graph.h"
#include "empire/view.h"

static const empire::GraphStyle NoMarker{
	empire::BlackWhiteVertexStyle,
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	{70, 70}};

template<typename T, typename U>
class GraphGui: public bwgui::Application
{
public:
	explicit GraphGui(empire::Graph<T, U> graph, empire::Grid const& grid);

	void OnClick(bwgui::Point<int> point) override {}
	void OnLoop() override {}
	void OnRender() override;
	
private:
	empire::Graph<T,U> graph_;
	empire::GraphView<T,U> view_;
	empire::Point position_{75, 75};
};

template<typename T, typename U>
GraphGui<T, U>::GraphGui(empire::Graph<T, U> graph, empire::Grid const& grid)
	: Application("graph_gui"),
	  graph_{std::move(graph)}
{
	view_ = empire::CreateView<T, U>(graph_, grid, NoMarker);

	empire::Colorizer<T, U>{view_}.paint_with(bwgui::Red, bwgui::Blue, bwgui::Green);
}

template<typename T, typename U>
void GraphGui<T, U>::OnRender()
{
	empire::DrawGraph(*this, view_, {75, 75});
}

int main()
{
	std::vector<char> values
	{
		'0',
		'1',
		'2', '3',
		'4', '5',
		'6', '7',
		'8', '9'
	};

	std::vector<empire::MetaLink<bool>> links
	{
		{0, 1}, {0, 2}, {0, 3},
		{1, 0}, {1, 6}, {1, 7},
		{2, 0}, {2, 4}, {2, 8},
		{3, 0}, {3, 5}, {3, 9},
		{4, 2}, {4, 5}, {4, 7},
		{5, 3}, {5, 4}, {5, 6},
		{6, 1}, {6, 5}, {6, 8},
		{7, 1}, {7, 4}, {7, 9},
		{8, 2}, {8, 6}, {8, 9},
		{9, 3}, {9, 7}, {9, 8}
	};

	auto graph = empire::make_graph(values, links);

	constexpr auto N = std::nullopt;
	empire::Grid grid{
		{ N, N, 0, N, N },
		{ 2, N, 1, N, 3 },
		{ N, 4, N, 5, N },
		{ N, 6, N, 7, N },
		{ 8, N, N, N, 9 }
	};

	GraphGui app{std::move(graph), grid};

	return app.Execute();
}
