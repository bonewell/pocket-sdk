#include "bwgui/action.h"
#include "bwgui/application.h"

#include "core/random.h"

#include "empire/graph.h"
#include "empire/view.h"

#include <vector>


constexpr empire::GraphStyle IdleStyle{
	{empire::BlackWhiteFontStyle, bwgui::Black, bwgui::White, 50.0},
	{{bwgui::Black, 0.0}, bwgui::Gray},
	{-10.0, -10.0}
};

class Anime
{
public:
	void Init(int index) { next_ = 0; index_ = index; steps_.clear(); }
	void AddStep(int from, int to) { steps_.emplace_back(from, to); }
	void SetStyle(empire::GraphStyle const& style) { style_ = style; }
	template<typename T, typename U = int>
	void Update(empire::GraphView<T, U>& view);
	bool IsFinished() const { return next_ >= steps_.size(); }
	int index() const { return index_; }
private:
	std::vector<std::pair<int, int>> steps_;
	empire::GraphStyle style_{empire::BlackWhiteGraphStyle};
	int next_{0};
	int index_{0};
};

template<typename T, typename U>
void Anime::Update(empire::GraphView<T, U>& view)
{
	if (IsFinished()) return;
	auto [from, to] = steps_[next_];
	view.vertexes[to].style.border = style_.vertex.border;
	view.vertexes[to].style.background = style_.vertex.background;
	if (from > -1)
	{
		auto& edge_from_to = view.vertexes[from].get_edge(to);
		edge_from_to.style.color = style_.edge.color;
		auto& edge_to_from = view.vertexes[to].get_edge(from);
		edge_to_from.style.color = style_.edge.color;
	}
	index_ = to;
	++next_;
}

enum class Stage{ Idle, Start, Depth, Width };

template<typename T>
class GraphGui: public bwgui::Application
{
public:
	explicit GraphGui(empire::Graph<T> graph, empire::Grid const& grid);

	void OnClick(bwgui::Point<int> point) override;
	void OnLoop() override;
	void OnRender() override;
	
private:
	void Update();

	empire::Graph<T> graph_;
	empire::GraphView<T> view_;
	empire::Point position_{250, 100};
	Anime anime_;
	Stage stage_{Stage::Idle};
	std::vector<bwgui::Color> colors_;
	int current_color_{0};
};

template<typename T>
GraphGui<T>::GraphGui(empire::Graph<T> graph, empire::Grid const& grid)
	: Application("graph_gui", 50),
	  graph_{std::move(graph)}
{
	view_ = empire::CreateView<T>(graph_, grid, IdleStyle);
	anime_.SetStyle(IdleStyle);
	colors_ = {
		bwgui::Silver, bwgui::Red, bwgui::Crimson, bwgui::Lime,
		bwgui::DarkGreen, bwgui::Green, bwgui::GreenYellow,
		bwgui::LightGreen, bwgui::Blue, bwgui::DodgerBlue, bwgui::DeepSkyBlue, bwgui::LightBlue,
		bwgui::MediumSlateBlue, bwgui::BlueViolet,
		bwgui::Yellow, bwgui::Gold, bwgui::Orange
	};
}

template<typename T>
void GraphGui<T>::OnClick(bwgui::Point<int> point)
{
	int index = view_.locate_vertex({point.x - position_.x, point.y - position_.y});
	if (index == -1) return;
	anime_.Init(index);
	view_.reset_styles();
	random_elements(colors_);
	current_color_ = 0;
	stage_ = Stage::Start;
}

template<typename T>
void GraphGui<T>::OnLoop()
{
	Update();	
	anime_.Update(view_);
}

template<typename T>
void GraphGui<T>::OnRender()
{
	empire::DrawGraph(*this, view_, position_);
}

template<typename T>
void GraphGui<T>::Update()
{
	switch (stage_)
	{
		case Stage::Idle: break;
		case Stage::Start: stage_ = Stage::Depth; break;
		case Stage::Depth: {
			if (anime_.IsFinished())
			{
				auto style = IdleStyle;
				style.vertex.background = colors_[current_color_];
				anime_.SetStyle(style);
				graph_.depth_loop(anime_.index(),
					[this](int from, int to){ anime_.AddStep(from, to); });
				current_color_ = (current_color_ + 1) % colors_.size();
				stage_ = Stage::Width;
			}
		} break;
		case Stage::Width: {
			if (anime_.IsFinished())
			{
				auto style = IdleStyle;
				style.vertex.background = colors_[current_color_];
				anime_.SetStyle(style);
				graph_.width_loop(anime_.index(),
					[this](int from, int to){ anime_.AddStep(from, to); });
				current_color_ = (current_color_ + 1) % colors_.size();
				stage_ = Stage::Depth;
			}
		} break;
	}
}

int main()
{
	std::vector<char> values
	{
		'I', 'N', 'T', 'E', 'R', 'N', 'A', 'T', 'I', 'O', 'N', 'A', 'L', ' ',
		'W', 'O', 'M', 'E', 'N', 'S', ' ', 'D', 'A', 'Y', '!'
	};

	const std::vector<empire::MetaLink<>> links
	{
		{0, 24}, {24, 0}, {0, 1}, {1, 0}, {1, 2}, {2, 1}, {2, 3}, {3, 2}, {3, 4}, {4, 3},
		{4, 5}, {5, 4}, {21, 5}, {5, 21}, {20, 5}, {5, 20}, {5, 6}, {6, 5}, {6, 7}, {7, 6},
		{7, 8}, {8, 7}, {8, 9}, {9, 8}, {9, 10}, {10, 9}, {10, 11}, {11, 10},
		{11, 12}, {12, 11}, {12, 13}, {13, 12}, {13, 14}, {14, 13}, {14, 15}, {15, 14},
		{15, 16}, {16, 15}, {16, 17}, {17, 16}, {17, 18}, {18, 17}, {18, 19}, {19, 18},
		{19, 20}, {20, 19}, {21, 22}, {22, 21}, {22, 23}, {23, 22}, {23, 24}, {24, 23}
	};
	
	auto graph = empire::make_graph(values, links);

	constexpr auto N = std::nullopt;
	const empire::Grid grid{
		{  N,  N,  1,  0, 24,  N, N },
		{  N,  2,  N,  N,  N, 23, N },
		{  N,  3,  N,  N,  N, 22, N }, 
		{  N,  N,  4,  N, 21,  N, N },
		{  N,  N,  N,  5,  N,  N, N },
		{  N,  N, 20,  N,  6,  N, N },
		{  N, 19,  N,  N,  N,  7, N },
		{ 18,  N,  N,  N,  N,  N, 8 },
		{ 17,  N,  N,  N,  N,  N, 9 },
		{ 16,  N,  N,  N,  N,  N, 10 },
		{  N, 15,  N,  N,  N, 11, N },
		{  N,  N, 14, 13,  12, N, N }
	};

	GraphGui app{std::move(graph), grid};

	return app.Loop();
}
