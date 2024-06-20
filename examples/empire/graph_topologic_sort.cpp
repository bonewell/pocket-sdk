#include "bwgui/action.h"
#include "bwgui/application.h"

#include "empire/graph.h"
#include "empire/topologic.h"
#include "empire/view.h"

static const empire::GraphStyle NoWeight{
	empire::BlackWhiteVertexStyle,
	{empire::BlackWhiteMarkerStyle, {bwgui::Black, 0.0}, bwgui::Gray},
	empire::DefaultPadding};

template<typename T, typename U>
class GraphGui: public bwgui::Application
{
public:
	explicit GraphGui(empire::Graph<T, U> graph, empire::Grid const& grid);

	void OnClick(bwgui::Point<int> point) override {}
	void OnLoop() override;
	void OnRender() override;
	
private:
	empire::Graph<T,U> graph_;
	empire::GraphView<T,U> view_;
	empire::Point position_{75, 75};
	std::vector<int> path_;
	int next_{-1};
};

template<typename T, typename U>
GraphGui<T, U>::GraphGui(empire::Graph<T, U> graph, empire::Grid const& grid)
	: Application("graph_gui", 1000),
	  graph_{std::move(graph)}
{
	view_ = empire::CreateView<T, U>(graph_, grid, NoWeight);
	path_ = empire::topologic_sort(graph_);
	next_ = 0;
}

template<typename T, typename U>
void GraphGui<T, U>::OnLoop()
{
	if (next_ == -1 || next_ >= path_.size()) return;

	auto from = path_[next_];
	view_.vertexes[from].style.border = bwgui::Yellow;
	view_.vertexes[from].style.background = bwgui::Green;
	++next_;
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

	GraphGui app{std::move(graph), grid};

	return app.Loop();
}
