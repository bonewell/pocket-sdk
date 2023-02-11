#include "bwgui/action.h"
#include "bwgui/application.h"
#include "bwgui/shape.h"

#include "taiga/thread_tree.h"
#include "taiga/view.h"

#include <random>

constexpr taiga::TreeStyle NewYearTreeStyle
{
	{
		{ bwgui::Black, 9.0 },
		bwgui::Black,
		bwgui::Lime,
		50.0
	},
	bwgui::Lime,
	{ 40.0, bwgui::White},
	{ 6.0, 30.0 }
};

template<typename T, typename U, typename V>
void FillTree(
	taiga::Link<T, U> const& link,
	taiga::Link<taiga::Vertex<T>, V>& vertex,
	taiga::View<T>& view,
	std::function<double(double, double)> op,
	taiga::Point const& p = {})
{
	if (!link) return;

	taiga::Point center
	{
		op(p.x, view.style.vertex.radius + view.style.padding.horizontal),
		p.y + view.style.vertex.D() + view.style.padding.vertical
	};

	view.add_vertex(std::forward<T>(link->value));
	vertex->value.center = center;

	::FillTree<T, U, V>(link->left, vertex->left, view, std::minus<>{}, center);
	::FillTree<T, U, V>(link->right, vertex->right, view, std::plus<>{}, center);
}

template<typename T>
taiga::ThreadTreeView<T> CreateView(taiga::ThreadLink<T> const& tree, taiga::TreeStyle style)
{
	taiga::ThreadTreeView<T> view;
	view.style = std::move(style);

	view.add_vertex(std::forward<T>(tree->value));

	::FillTree<T>(tree->left, view.root->left, view, std::minus<>{});
	::FillTree<T>(tree->right, view.root->right, view, std::plus<>{});

	return view;
}

struct Letter
{
	Letter() = default;
	Letter(int p, char v, bwgui::Color c): position{p}, value{v}, color{c} {}
	int position{0};
	char value{'0'};
	bwgui::Color color{bwgui::Black};
	int tiks{0};
	operator char() const { return value; }
};
bool operator<(Letter const& a, Letter const& b)
{
	return a.position < b.position;
}

template<typename T>
class TreeGui: public bwgui::Application
{
public:
	explicit TreeGui(taiga::ThreadTree<T> tree);

	void OnLoop() override;
	void OnRender() override;
	void OnClick(bwgui::Point<int> p) override;
	
private:
	taiga::ThreadTreeView<T> view_;
	std::default_random_engine engine_{static_cast<unsigned>(time(nullptr))};
	std::uniform_int_distribution<> show_{1, 100};
	std::uniform_int_distribution<> hide_{1, 50};
	taiga::Point zero_{500.0, 100.0};
};

template<typename T>
TreeGui<T>::TreeGui(taiga::ThreadTree<T> tree)
	: Application("tree_gui", 50)
{
	view_ = ::CreateView(tree, NewYearTreeStyle);
}

template<typename T>
void TreeGui<T>::OnLoop()
{
	taiga::ModifyFunctor<taiga::Vertex<T, false>> set_color = [this](auto& v)
	{
		if (v.value.tiks == 0)
		{
			auto& color = v.style.label.color;
			if (color == bwgui::Black)
			{
				color = v.value.color;
				v.value.tiks = this->show_(this->engine_);
			}
			else
			{
				color = bwgui::Black;
				v.value.tiks = this->hide_(this->engine_);
			}
		}
		--v.value.tiks;
	};
	taiga::apply(view_.root, set_color);
}

template<typename T>
void TreeGui<T>::OnRender()
{
	taiga::DrawThreadTree(*this, view_, zero_);
}

template<typename T>
void TreeGui<T>::OnClick(bwgui::Point<int> p)
{
	taiga::Point point{static_cast<double>(p.x - zero_.x), static_cast<double>(p.y - zero_.y)};
	auto& link = taiga::locate(view_.root, point);
	if (link)
	{
		link->value.style.background = link->value.style.label.color;
	}
}

int main()
{
	std::vector<Letter> letters;
	{
						   //          1         2         3
						   //012345678901234567890123456789012
		std::string words = "MERRY CRISTMAS AND HAPPY NEW YEAR";
	
		std::vector<int> positions
		{
			16, 15, 13, 14, 10, 11, 12, 6, 7, 8, 9, 3, 4, 5, 1, 2, 0,
			17, 19, 18, 22, 21, 20, 26, 25, 24, 23, 29, 28, 27, 31, 30, 32
		};
		
		std::vector<bwgui::Color> colors
		{
			bwgui::Crimson,
			bwgui::DeepSkyBlue,
			bwgui::BlueViolet,
			bwgui::Orange,
			bwgui::GreenYellow
		};
	
		for (int i = 0; i < positions.size(); ++i)
		{
			auto p = positions[i];
			letters.emplace_back(p, words[p], colors[i % colors.size()]);
		}
	}
	
	auto thread_tree = taiga::make_thread_tree<Letter>(letters);

	TreeGui app{std::move(thread_tree)};

	return app.Loop();
}
