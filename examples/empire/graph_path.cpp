#include "apps/graph_gui.h"

#include "empire/graph.h"

class App: public GraphGui<empire::Graph<char>>
{
public:
	using GraphGui<empire::Graph<char>>::GraphGui;

	void OnTap(vertex_type& vertex) override;
	void OnLoop() override;
	
private:
	vertex_type* from_{nullptr};

	using link_type = typename graph_type::link_type;
	std::vector<link_type*> path_;
	int next_{-1};
};

void App::OnTap(vertex_type& vertex)
{
	if (!from_ || from_ == &vertex) 
	{
		from_ = &vertex;
		reset();
		return;
	}
	
	auto from = from_->node;
	auto to = vertex.node;
	path_ = graph().find_path(from, to,
//		empire::Traverse::Width
//		empire::Traverse::Depth
//		empire::Traverse::Mark
		empire::Traverse::Remark
	);
	next_ = 0;
	from_ = nullptr;
}

void App::OnLoop()
{
	if (from_)
	{
		from_->style.background = bwgui::Gray;	
	}
	
	if (path_.empty()) return;

	if (0 <= next_ && next_ < path_.size())
	{
		const auto& link = path_[next_++];

		vertex(link->from).style.border = bwgui::Yellow;
		vertex(link->from).style.background = bwgui::Green;

		vertex(link->from).get_edge(link->to).style.color = bwgui::Yellow;
		vertex(link->to).get_edge(link->from).style.color = bwgui::Yellow;
	}
	if (next_ == path_.size())
	{
		const auto& link = path_.back();
		vertex(link->to).style.border = bwgui::Yellow;
		vertex(link->to).style.background = bwgui::Green;
		next_++;
	}
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

	App app{std::move(graph), grid};

	return app.Loop();
}
