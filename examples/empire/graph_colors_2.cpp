#include "bwgui/action.h"
#include "bwgui/application.h"

#include "empire/colorizer.h"
#include "empire/graph.h"
#include "empire/view.h"

static const empire::GraphStyle NoMarker{
	empire::BlackWhiteVertexStyle,
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	empire::DefaultPadding};

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

	//empire::Colorizer<T, U>{view_}.paint_with(bwgui::Red);
	empire::Colorizer<T, U>{view_}.paint_with(bwgui::Red, bwgui::Blue);
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
		// {7, 12}, {12, 7}, // error

		{0, 1}, {0, 2}, {0, 3},
		{1, 0},
		{2, 0}, {2, 6},
		{3, 0}, {3, 4}, {3, 5}, {3, 6}, {3, 7},
		{4, 3},
		{5, 3}, {5, 9},
		{6, 2}, {6, 3},
		{7, 3}, {7, 8}, {7, 10}, {7, 11}, {7, 15},
		{8, 7},
		{9, 5},
		{10, 7}, {10, 12}, {10, 13},
		{11, 7}, {11, 12}, {11, 13},
		{12, 10}, {12, 11},
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

	GraphGui app{std::move(graph), grid};

	return app.Execute();
}
