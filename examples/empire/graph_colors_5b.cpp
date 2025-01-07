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
		empire::Colorizer{view()}.paint_with(
			bwgui::Red,
			bwgui::Blue,
			bwgui::Green,
			bwgui::Orange,
			bwgui::BlueViolet
		);
	}

};

int main()
{
	std::vector<char> values
	{
		          'L',
		     'G',      'H',
		          'C',
		'K', 'B',      'D', 'I',
		          'A',
		     'F',      'E',
		          'J'
	};
	std::vector<empire::MetaLink<bool>> links
	{
		{0, 1}, {0, 2}, {0, 4}, {0, 7}, {0, 11},
		{1, 0}, {1, 2}, {1, 3}, {1, 4}, {1, 5},
		{2, 0}, {2, 1}, {2, 3}, {2, 6}, {2, 7},
		{3, 1}, {3, 2}, {3, 5}, {3, 6}, {3, 8},
		{4, 0}, {4, 1}, {4, 5}, {4, 9}, {4, 11},
		{5, 1}, {5, 3}, {5, 4}, {5, 8}, {5, 9},
		{6, 2}, {6, 3}, {6, 7}, {6, 8}, {6, 10},
		{7, 0}, {7, 2}, {7, 6}, {7, 10}, {7, 11},
		{8, 3}, {8, 5}, {8, 6}, {8, 9}, {8, 10},
		{9, 4}, {9, 5}, {9, 8}, {9, 10}, {9, 11},
		{10, 6}, {10, 7}, {10, 8}, {10, 9}, {10, 11},
		{11, 0}, {11, 4}, {11, 7}, {11, 9}, {11, 10}
	};

	auto graph = empire::make_graph(values, links);

	constexpr auto N = std::nullopt;
	empire::Grid grid{
		{ N, N, 0, N, N },
		{ 1, N, N, N, 2 },
		{ 4, 5, 3, 6, 7 },
		{ N, N, 8, 10, N },
		{ N, 9, N, N, N },
		{ N, N, N, 11, N }
	};

	App app{std::move(graph), grid};

	return app.Execute();
}
