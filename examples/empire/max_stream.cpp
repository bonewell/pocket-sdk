#include "apps/graph_gui.h"

#include "empire/maximizer.h"

class App: public GraphGui<empire::Graph<char, empire::Pipe>>
{
public:
	using GraphGui<empire::Graph<char, empire::Pipe>>::GraphGui;	
};

int main()
{
	std::vector<char> values
	{
		'A', 'B', 'C',
		'D', 'E', 'F'
	};

	std::vector<empire::MetaLink<empire::Pipe>> links
	{
		{0, 1, 3}, {0, 3, 2},
		{1, 2, 2}, {1, 4, 2},
		{2, 5, 3},
		{3, 4, 2},
		{4, 5, 3}
	};
/*
	std::vector<empire::MetaLink<empire::Pipe>> links
	{
		{0, 1, {3, 3}}, {0, 3, {2, 1}},
		{1, 2, {2, 1}}, {1, 4, {2, 2}},
		{2, 5, {3, 1}},
		{3, 4, {2, 1}},
		{4, 5, {3, 3}}
	};
*/
	auto graph = empire::maximize_stream(empire::make_graph(std::move(values), std::move(links)));

	empire::Grid grid{
		{ 0, 1, 2 },
		{ 3, 4, 5 }
	};

	App app{std::move(graph), grid};

	return app.Loop();
}
