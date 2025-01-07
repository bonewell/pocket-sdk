#include "apps/graph_gui.h"

#include "empire/colorizer.h"
#include "empire/graph.h"

static const empire::GraphStyle NoMarker{
	empire::BlackWhiteVertexStyle,
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	{70, 70}};

class App: public GraphGui<empire::Graph<char, bool>>
{
public:
	App(graph_type graph, empire::Grid const& grid)
		: GraphGui(std::move(graph), grid, NoMarker)
	{
		empire::Colorizer{view()}.paint_with(bwgui::Red, bwgui::Blue, bwgui::Green);
	}

};

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

	App app{std::move(graph), grid};

	return app.Execute();
}
