#include "apps/graph_gui.h"
#include "empire/graph.h"
#include "core/random.h"

#include <vector>

constexpr empire::GraphStyle IdleStyle{
	{empire::BlackWhiteFontStyle, bwgui::Black, bwgui::White, 50.0},
	{{false}, {bwgui::Black, 0.0}, bwgui::Gray},
	{-10.0, -10.0}
};

template<typename T>
class Anime
{
public:
	void Init(T const* node) { next_ = 0; node_ = node; steps_.clear(); }
	void AddStep(T const* from, T const* to)
	{ steps_.emplace_back(from, to); }
	void SetStyle(empire::GraphStyle const& style) { style_ = style; }
	template<typename A> void Update(A& app);
	bool IsFinished() const { return next_ >= steps_.size(); }
	T const* node() const { return node_; }

private:	
	std::vector<std::pair<T const*, T const*>> steps_;

	empire::GraphStyle style_{empire::BlackWhiteGraphStyle};
	int next_{0};
	T const* node_{nullptr};
};

template<typename T>
template<typename A>
void Anime<T>::Update(A& app)
{
	if (IsFinished()) return;
	auto [from, to] = steps_[next_++];

	app.vertex(from).style.border = style_.vertex.border;
	app.vertex(from).style.background = style_.vertex.background;

	app.vertex(to).style.border = style_.vertex.border;
	app.vertex(to).style.background = style_.vertex.background;

	app.vertex(from).get_edge(to).style.color = style_.edge.color;
	app.vertex(to).get_edge(from).style.color = style_.edge.color;

	node_ = to;
}

enum class Stage{ Idle, Start, Depth, Width };

class App: public GraphGui<empire::Graph<char>>
{
public:
	App(graph_type graph, empire::Grid const& grid);

	void OnTap(vertex_type& vertex) override;
	void OnLoop() override;
	
private:
	void Update();

	Anime<typename graph_type::node_type> anime_;
	Stage stage_{Stage::Idle};
	std::vector<bwgui::Color> colors_;
	int current_color_{0};
};

App::App(graph_type graph, empire::Grid const& grid)
	: GraphGui(std::move(graph), grid, IdleStyle, 50)
{
	anime_.SetStyle(IdleStyle);
	colors_ = {
		bwgui::Silver, bwgui::Red, bwgui::Crimson, bwgui::Lime,
		bwgui::DarkGreen, bwgui::Green, bwgui::GreenYellow,
		bwgui::LightGreen, bwgui::Blue, bwgui::DodgerBlue, bwgui::DeepSkyBlue, bwgui::LightBlue,
		bwgui::MediumSlateBlue, bwgui::BlueViolet,
		bwgui::Yellow, bwgui::Gold, bwgui::Orange
	};
}

void App::OnTap(vertex_type& vertex)
{	
	anime_.Init(vertex.node);
	reset();
	core::random_elements(colors_);
	current_color_ = 0;
	stage_ = Stage::Start;
}

void App::OnLoop()
{
	Update();	
	anime_.Update(*this);
}

void App::Update()
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
				graph().traverse(
					anime_.node(),
					[this](auto const& l){ anime_.AddStep(l->from, l->to); },
					empire::Traverse::Depth);
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
				graph().traverse(
					anime_.node(),
					[this](auto const& l){ anime_.AddStep(l->from, l->to); },
					empire::Traverse::Width);
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

	App app{std::move(graph), grid};

	return app.Loop();
}
