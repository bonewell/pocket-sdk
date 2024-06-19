#ifndef _COLORIZER_H_
#define _COLORIZER_H_

#include "view.h"

#include "../bwgui/color.h"
#include "../core/logger.h"

#include <queue>
#include <unordered_set>

namespace
{

using Colors = std::vector<bwgui::Color>;
	
template<typename T, typename U = int>
void print_colored(empire::Vertex<T, U> const& v)
{
	// std::cout << v.node->value << " colored " << v.style.background << '\n';
}

template<typename T, typename U = int>
void print_error(empire::Vertex<T, U> const& v)
{
	std::cerr << "Impossible to color " << v.node->value << '\n';
}

template<typename T, typename U = int>
using Vertexes = std::list<std::reference_wrapper<empire::Vertex<T, U>>>;

template<typename T, typename U = int>
class Simplifier
{
public:
	Simplifier(empire::GraphView<T, U>& view): view_{view} {}
	void simplify(int n);
	
	Vertexes<T, U> const& base() const { return base_; }
	Vertexes<T, U> const& extracted() const { return extracted_; }

private:
	std::pair<std::list<int>, std::vector<int>> init();
	Vertexes<T, U> extract(int limit, std::list<int>& indexes, std::vector<int> const& link_counts) const;
	void reduce_links(::Vertexes<T, U> const& vertexes, std::vector<int>& link_counts) const;
	void append_extracted(::Vertexes<T, U> const& vertexes);
	void do_base(std::list<int> const& indexes);

	empire::GraphView<T, U>& view_;
	Vertexes<T, U> base_;
	Vertexes<T, U> extracted_;
};


template<typename T, typename U>
void Simplifier<T, U>::simplify(int n)
{
	auto [indexes, link_counts] = init();

	bool done = true;
	do
	{
		auto vertexes = extract(n, indexes, link_counts);
		reduce_links(vertexes, link_counts);
		append_extracted(vertexes);
		done = vertexes.empty();
	} while (!done);

	do_base(indexes);
}

template<typename T, typename U>
std::pair<std::list<int>, std::vector<int>> Simplifier<T, U>::init()
{
	base_.clear();
	extracted_.clear();

	std::list<int> indexes;
	std::vector<int> link_counts;

	int n = view_.vertexes.size();	
	link_counts.reserve(n);

	for (int i = 0; i < n; ++i)
	{
		indexes.insert(indexes.end(), i);
		auto const& v = view_.vertexes[i];
		link_counts.push_back(v.edges.size());
	}
	
	return std::make_pair(std::move(indexes), std::move(link_counts));
}

template<typename T, typename U>
Vertexes<T, U> Simplifier<T, U>::extract(int limit,
	std::list<int>& indexes,
	std::vector<int> const& link_counts) const
{
	Vertexes<T, U> vertexes;
	for (auto it = std::begin(indexes); it != std::end(indexes);)
	{
		auto const i = *it;
		if (0 < link_counts[i] && link_counts[i] < limit)
		{
			auto& v = view_.vertexes[i];
			vertexes.emplace_front(v);
			it = indexes.erase(it);
		}
		else ++it;
	}
	return vertexes;
}

template<typename T, typename U>
void Simplifier<T, U>::append_extracted(::Vertexes<T, U> const& vertexes)
{
	extracted_.insert(extracted_.begin(), vertexes.begin(), vertexes.end());
}

template<typename T, typename U>
void Simplifier<T, U>::reduce_links(::Vertexes<T, U> const& vertexes, std::vector<int>& link_counts) const
{
	for (auto const& v: vertexes)
	{
		for (auto const& e: v.get().edges) --link_counts[e.link->to->index];
	}
}

template<typename T, typename U>
void Simplifier<T, U>::do_base(std::list<int> const& indexes)
{
	for (auto const i: indexes)
	{
		auto& v = view_.vertexes[i];
		base_.emplace_back(v);
	}
}

} // namespace

namespace empire
{

template<typename T, typename U = int>
class Colorizer
{
public:
	Colorizer(empire::GraphView<T, U>& view): view_{view} {}

	void paint_with(bwgui::Color const& color1);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3, bwgui::Color const& color4);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3, bwgui::Color const& color4, bwgui::Color const& color5);
	void paint_with(Colors const& colors);
	
private:
	bool has_colored_neighbour(empire::Vertex<T, U> const& v, bwgui::Color const& color);
	bool paint_one_with(empire::Vertex<T, U>& v, Colors const& colors);
	void paint_with(::Simplifier<T, U> simplifier, Colors const& colors);

	empire::GraphView<T, U>& view_;
};

template<typename T, typename U>
bool Colorizer<T, U>::has_colored_neighbour(empire::Vertex<T, U> const& v, bwgui::Color const& color)
{
	for (auto const& e: v.edges)
	{
		auto const& neighbour = view_.vertexes[e.link->to->index];
		if (neighbour.style.background == color)
		{
			return true;
		}
	}
	return false;
}

template<typename T, typename U>
bool Colorizer<T, U>::paint_one_with(empire::Vertex<T, U>& v, Colors const& colors)
{
	for (auto const& color: colors)
	{
		if (!has_colored_neighbour(v, color))
		{
			v.style.background = color;
			print_colored(v);
			return true;
		}
	}
	print_error(v);
	return false;
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(::Simplifier<T, U> simplifier, Colors const& colors)
{
	simplifier.simplify(colors.size());

	for (auto& v: simplifier.base())
	{
		auto const ok = paint_one_with(v.get(), colors);
		if (!ok) return;
	}

	for (auto& v: simplifier.extracted())
	{
		auto const ok = paint_one_with(v.get(), colors);
		if (!ok) return;
	}
}
	
template<typename T, typename U>
void Colorizer<T, U>::paint_with(bwgui::Color const& color)
{
	for (auto& v: view_.vertexes) v.style.background = color;
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(bwgui::Color const& color1, bwgui::Color const& color2)
{
	if (view_.vertexes.empty()) return;

	auto& v = view_.vertexes.front();
	v.style.background = color1;
	print_colored(v);

	std::queue<Vertex<T, U>const*const> colored;
	colored.push(&v);
	
	while (!colored.empty())
	{
		auto v = colored.front();
		colored.pop();
		
		auto const& color = v->style.background == color1 ? color2 : color1;

		for (auto const& e: v->edges)
		{
			auto& to = view_.vertexes[e.link->to->index];
			if (to.style.background == v->style.background)
			{
				std::cerr << "Impossible to color: " << v->node->value << " and " << to.node->value << " have the same color" << std::endl;
				
				return;
			}
			if (to.style.background == color)
			{
				// already colored right
			}
			else
			{
				to.style.background = color;
				colored.push(&to);
				print_colored(to);
			}
		}
	}
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3)
{
	paint_with(::Simplifier<T, U>{view_}, {color1, color2, color3});
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3,
	bwgui::Color const& color4)
{
	paint_with(::Simplifier<T, U>{view_}, {color1, color2, color3, color4});
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3,
	bwgui::Color const& color4,
	bwgui::Color const& color5)
{
	// TODO - do specific simplifier
	paint_with(::Simplifier<T, U>{view_}, {color1, color2, color3, color4, color5});
}

template<typename T, typename U>
void Colorizer<T, U>::paint_with(Colors const& colors)
{
	for (auto& v: view_.vertexes)
	{
		auto const ok = paint_one_with(v, colors);
		if (!ok) return;
	}
}

} // namespace empire

#endif // _COLORIZER_H_
