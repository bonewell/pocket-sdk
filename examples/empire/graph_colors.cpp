#include "apps/graph_gui.h"

#include "empire/colorizer.h"
#include "empire/graph.h"

static const empire::GraphStyle NoMarker{
	empire::BlackWhiteVertexStyle,
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	empire::DefaultPadding};

class App: public GraphGui<empire::Graph<char>>
{
public:
	App(graph_type graph, empire::Grid const& grid)
		: GraphGui(std::move(graph), grid, NoMarker)
	{
		empire::Colorizer{view()}.paint_with({
			bwgui::Red,
			bwgui::Blue,
			bwgui::Green,
			bwgui::Orange,
			bwgui::BlueViolet,
			bwgui::DeepSkyBlue});
	}
};

int main()
{
	std::vector<char> values
	{
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};

	std::vector<empire::MetaLink<>> links
	{
		{0, 1}, {0, 2}, {0, 3},
		{1, 0}, {1, 2}, {1, 8},
		{2, 0}, {2, 1}, {2, 3}, {2, 6}, {2, 8},
		{3, 0}, {3, 2}, {3, 4}, {3, 5}, {3, 6}, {3, 7}, {3, 8},
		{4, 3}, {4, 7}, {4, 10}, {4, 11}, {4, 12}, {4, 13},
		{5, 3}, {5, 9},
		{6, 2}, {6, 3},
		{7, 3}, {7, 4}, {7, 8}, {7, 10}, {7, 11}, {7, 12}, {7, 13}, {7, 15},
		{8, 1}, {8, 2}, {8, 3}, {8, 7},
		{9, 5},
		{10, 4}, {10, 7}, {10, 11}, {10, 12}, {10, 13},
		{11, 4}, {11, 7}, {11, 10}, {11, 12}, {11, 13},
		{12, 4}, {12, 7}, {12, 10}, {12, 11}, {12, 13},
		{13, 4}, {13, 7}, {13, 10}, {13, 11}, {13, 12}, {13, 14},
		{14, 13},
		{15, 7}
	};

	auto graph = empire::make_graph(values, links);

	constexpr auto N = std::nullopt;
	empire::Grid grid{
		{ 6, 0, N, 9, 5, N},
		{ 1, 2, 3, N, 4, 13},
		{ 8, 7, 10, N, N, N},
		{ 15, 12, 11, 14, N, N}
	};

	App app{std::move(graph), grid};

	return app.Execute();
}
