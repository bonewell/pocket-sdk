#include "apps/graph_gui.h"

#include "empire/maximizer.h"

using Graph = empire::Graph<char, empire::Pipe>;

class App: public GraphGui<Graph>
{
public:
	using GraphGui<Graph>::GraphGui;
};

int main()
{
	std::vector<char> values
	{
		'A', 'B', 'C', 'D',
		'E', 'F', 'G', 'H',
		'I', 'J', 'K', 'L',
//		'M', 'N', 'O', 'P'
		'M', 'N', 'P', 'O'
	};

	std::vector<empire::MetaLink<empire::Pipe>> links
	{
		{0, 1, 4}, {1, 2, 4}, {2, 3, 0},
		{0, 4, 5}, {1, 5, 2}, {2, 6, 2}, {3, 7, 2},
		{4, 5, 2}, {5, 6, 0}, {6, 7, 2},
		{4, 8, 2}, {5, 9, 0}, {6, 10, 2}, {7, 11, 2},
		{8, 9, 2}, {9, 10, 2}, {10, 11, 2},
//		{8, 12, 2}, {9, 13, 0}, {10, 14, 2}, {11, 15, 2},
		{8, 12, 2}, {9, 13, 0}, {10, 15, 2}, {11, 14, 2},
//		{12, 13, 0}, {13, 14, 2}, {14, 15, 2}
		{12, 13, 0}, {13, 15, 2}, {15, 14, 2}
	};

	auto graph = empire::make_graph(std::move(values), std::move(links));
	auto stream = empire::maximize_stream(std::move(graph));
	auto ab = empire::cut_maximized_stream(std::move(stream));

	empire::Grid grid{
		{ 0,   1,  2,  3 },
		{ 4,   5,  6,  7 },
		{ 8,   9, 10, 11 },
//		{ 12, 13, 14, 15 }
		{ 12, 13, 15, 14 }
	};

	App app{std::move(ab), grid};

	return app.Execute();
}
