#include "bwgui/action.h"
#include "bwgui/application.h"

#include "empire/graph.h"
#include "empire/view.h"

template<typename T>
class GraphGui: public bwgui::Application
{
public:
	explicit GraphGui(empire::Graph<T> graph, empire::Grid const& grid);

	void OnClick(bwgui::Point<int> point) override;
	void OnLoop() override;
	void OnRender() override;
	
private:
	empire::Graph<T> graph_;
	empire::GraphView<T> view_;
	empire::Point position_{75, 75};
	std::optional<int> from_;
	std::vector<int> path_;
	int next_{-1};
};

template<typename T>
GraphGui<T>::GraphGui(empire::Graph<T> graph, empire::Grid const& grid)
	: Application("graph_gui", 300),
	  graph_{std::move(graph)}
{
	view_ = empire::CreateView<T>(graph_, grid);
}

template<typename T>
void GraphGui<T>::OnClick(bwgui::Point<int> point)
{
	int index = view_.locate_vertex({point.x - position_.x, point.y - position_.y});
	if (index == -1) return;
	if (!from_) 
	{
		from_ = index;
		return;
	}
	
	path_ = graph_.find_path(*from_, index, empire::remark_traverse<T>);
	view_.reset_styles();
	next_ = 0;
	from_ = std::nullopt;
}

template<typename T>
void GraphGui<T>::OnLoop()
{
	if (next_ == -1 || next_ >= path_.size()) return;

	auto from = path_[next_];
	view_.vertexes[from].style.border = bwgui::Yellow;
	view_.vertexes[from].style.background = bwgui::Green;
	
	if (next_ >= path_.size() - 1) return;
	auto to = path_[next_ + 1];
	auto& edge_from_to = view_.vertexes[from].get_edge(to);
	edge_from_to.style.color = bwgui::Yellow;
	auto& edge_to_from = view_.vertexes[to].get_edge(from);
	edge_to_from.style.color = bwgui::Yellow;

	++next_;
}

template<typename T>
void GraphGui<T>::OnRender()
{
	empire::DrawGraph(*this, view_, {75, 75});
}

int main()
{
	std::vector<char> values
	{
		'A', 'B', 'C', 'D', 'E',
		'F', 'G', 'H', 'I', 'J',
		'K', 'L', 'M', 'N', 'O',
		'P', 'Q', 'R', 'S', 'T'
	};

	std::vector<empire::MetaLink<>> links
	{
		{0, 1, 83}, {1, 0, 83},
		{0, 5, 40}, {5, 0, 40},
		{0, 6, 114}, {6, 0, 114},
		{1, 2, 91}, {2, 1, 91},
		{1, 6, 76}, {6, 1, 76},
		{2, 3, 83}, {3, 2, 83},
		{2, 7, 78}, {7, 2, 78},
		{3, 4, 87}, {4, 3, 87},
		{3, 8, 83}, {8, 3, 83},
		{4, 9, 84}, {9, 4, 84},

		{5, 6, 44}, {6, 5, 44},
		{5, 10, 80}, {10, 5, 80},
		{6, 7, 96}, {7, 6, 96},
		{6, 11, 79}, {11, 6, 79},
		{7, 8, 86}, {8, 7, 86},
		{8, 9, 92}, {9, 8, 92},
		{8, 13, 74}, {13, 8, 74},
		{8, 14, 126}, {14, 8, 126},
		{9, 14, 76}, {14, 9, 76},

		{10, 11, 89}, {11, 10, 89},
		{10, 15, 73}, {15, 10, 73},
		{11, 12, 99}, {12, 11, 99},
		{11, 15, 120}, {15, 11, 120},
		{11, 16, 71}, {16, 11, 71},
		{13, 14, 91}, {14, 13, 91},
		{13, 18, 76}, {18, 13, 76},
		{14, 19, 73}, {19, 14, 73},

		{15, 16, 91}, {16, 15, 91},
		{16, 17, 98}, {17, 16, 98},
		{18, 19, 97}, {19, 18, 97}
	};

	auto graph = empire::make_graph(values, links);
	empire::Grid grid{
		{ 0,  1,  2,  3,  4},
		{ 5,  6,  7,  8,  9},
		{10, 11, 12, 13, 14},
		{15, 16, 17, 18, 19}
	};

	GraphGui app{std::move(graph), grid};

	return app.Loop();
}
