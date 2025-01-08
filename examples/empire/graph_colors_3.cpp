#include "apps/graph_gui.h"

#include "empire/colorizer.h"
#include "empire/graph.h"

static const empire::GraphStyle NoMarker{
	empire::BlackWhiteVertexStyle,
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	empire::DefaultPadding};

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
		'0', '1', '2', '3',
		'4', '5', '6', '7',
		'8', '9', 'A', 'B',
		'C', 'D', 'E', 'F'
	};

	std::vector<empire::MetaLink<bool>> links
	{
		// {10, 12}, {12, 10}, // error

		{0, 1}, {0, 2}, {0, 3},
		{1, 0},
		{2, 0}, {2, 6},
		{3, 0}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
		{4, 3},
		{5, 3}, {5, 9},
		{6, 2}, {6, 3},
		{7, 3}, {7, 8}, {7, 10}, {7, 11}, {7, 12}, {7, 15},
		{8, 7},
		{9, 5},
		{10, 7}, {10, 11}, {10, 13},
		{11, 7}, {11, 10}, {11, 12}, {11, 13},
		{12, 7}, {12, 11},
		{13, 10}, {13, 11}, {13, 14},
		{14, 13},
		{15, 7}
	};

	auto graph = empire::make_graph(values, links);

	constexpr auto N = std::nullopt;
	empire::Grid grid{
		{ N, N, 0, N, 5},
		{ 1, 2, 3, 4, 9},
		{ 6, 8, 7, 10, 13},
		{ N, 15, 12, 11, 14}
	};

	App app{std::move(graph), grid};

	return app.Loop();
}
