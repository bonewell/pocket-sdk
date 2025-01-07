#include "apps/graph_gui.h"

#include "empire/topologic.h"
#include "empire/graph.h"

static const empire::GraphStyle NoWeight{
	empire::BlackWhiteVertexStyle,
	{empire::BlackWhiteMarkerStyle, {bwgui::Black, 0.0}, bwgui::Gray},
	empire::DefaultPadding};

class App: public GraphGui<empire::Graph<char, bool>>
{
public:
	using GraphGui<empire::Graph<char, bool>>::GraphGui;

	void OnLoop() override;
	
private:
	vertex_type* from_{nullptr};

	using node_type = typename graph_type::node_type;
	std::vector<node_type const*> path_{empire::topologic_sort(graph())};
	int next_{0};
};

void App::OnLoop()
{
	if (0 <= next_ && next_ < path_.size())
	{
		auto from = path_[next_++];
		vertex(from).style.border = bwgui::Yellow;
		vertex(from).style.background = bwgui::Green;
	}
}

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
		// graph with following task (invertion of dependencies)
		{1, 0},
		{2, 0},
		{3, 0}, {3, 4}, {3, 5},
		{4, 0},
		{5, 0},
		{6, 2}, {6, 3},
		{7, 3}, {7, 8}, {7, 15},
		{8, 3},
		{9, 5},
		{10, 7}, {10, 12},
		{11, 7}, {11, 12},
		{12, 7},
		{13, 10}, {13, 11},
		{14, 13}

/*
		{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5},
		{2, 6},
		{3, 6}, {3, 7}, {3, 8},
		{4, 3},
		{5, 3}, {5, 9},
		{7, 10}, {7, 11}, {7, 12},
		{8, 7},
		{10, 13},
		{11, 13},
		{12, 10}, {12, 11},
		{13, 14},
		{15, 7}
*/
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
