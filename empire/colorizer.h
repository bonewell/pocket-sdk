#ifndef _COLORIZER_H_
#define _COLORIZER_H_

#include "view.h"

#include "../bwgui/color.h"
#include "../core/logger.h"

#include <queue>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace
{

using Colors = std::vector<bwgui::Color>;
	
template<typename T>
void print_colored(T const& v)
{
	// std::cout << v.node->value << " colored " << v.style.background << '\n';
}

template<typename T>
void print_error(T const& v)
{
	std::cerr << "Impossible to color " << v.node->value << '\n';
}

template<typename T>
bool operator==(std::reference_wrapper<T> const& lhs, std::reference_wrapper<T> const& rhs)
{ return lhs.get() == rhs.get(); }

template<typename T>
struct Triple { T k, m, n; };

template<typename T>
struct ColorNode
{
	using color_node_ref = std::reference_wrapper<ColorNode<T>>;
	
	struct Hash
	{
		std::size_t operator()(color_node_ref const& v) const noexcept
		{ return v.get().hash(); }
	};

	using ColorNodeRefUSet = std::unordered_set<color_node_ref, Hash>;

	ColorNode() = delete;
	explicit ColorNode(T* n): node{n} {}
	ColorNode(color_node_ref k, color_node_ref m, color_node_ref n);

	bool is_packed() const { return merged_.has_value(); }
	Triple<color_node_ref> unpack() const { return *merged_; }

	std::size_t hash() const;

	bool is_neighbor(color_node_ref v) const { return neighbors.count(v) == 1; }

	void bind(color_node_ref v) { neighbors.insert(v); }
	void unbind(color_node_ref v) { neighbors.erase(v); }
	void bind();
	void unbind();

	T* node;
	ColorNodeRefUSet neighbors;

private:
	std::optional<Triple<color_node_ref>> merged_;
};

template<typename T>
bool operator==(ColorNode<T> const& lhs, ColorNode<T> const& rhs)
{ return lhs.node == rhs.node; }

template<typename T>
ColorNode<T>::ColorNode(color_node_ref k, color_node_ref m, color_node_ref n)
{
	merged_ = Triple<color_node_ref>{k, m, n};
	for (auto v: m.get().neighbors) bind(v);
	for (auto v: n.get().neighbors) bind(v);
	unbind(k);
}

template<typename T>
void ColorNode<T>::bind()
{
	color_node_ref self{*this};
	for (auto& n: neighbors)
	{
		n.get().bind(self);
	}
}

template<typename T>
void ColorNode<T>::unbind()
{
	color_node_ref self{*this};
	for (auto& n: neighbors)
	{
		n.get().unbind(self);
	}
}

template<typename T>
std::size_t ColorNode<T>::hash() const
{
	if (merged_)
	{
		auto [k, m, n] = *merged_;
		auto h = k.get().hash() ^ (m.get().hash() << 1);
		return h ^ (n.get().hash() << 2);
	}
	return std::hash<T const*>{}(node);
}

template<typename T>
class Simplifier
{
public:
	using node_type = typename T::node_type;
	using color_node_ref = typename ColorNode<node_type>::color_node_ref; // TODO - use const node_type
	using ColorNodeRefList = std::list<color_node_ref>;

	Simplifier(T& view);

	void simplify(int n);
	
	ColorNodeRefList const& base() const { return base_; }
	ColorNodeRefList const& extracted() const { return extracted_; }

private:
	ColorNodeRefList init();
	bool extract(int limit, ColorNodeRefList& vertexes);
	bool pack(ColorNodeRefList& vertexes);

	void do_base(ColorNodeRefList& vertexes)
	{ base_ = std::move(vertexes); }

	ColorNodeRefList pull(int limit, ColorNodeRefList& vertexes) const;
	void unbind(ColorNodeRefList const& vertexes) const;

	void fold(ColorNodeRefList const& vertexes)
	{ extracted_.insert(extracted_.begin(), vertexes.begin(), vertexes.end()); }

	std::optional<Triple<color_node_ref>> pull_triple(ColorNodeRefList& vertexes) const;
	color_node_ref merge(color_node_ref k, color_node_ref m, color_node_ref n);

	std::vector<ColorNode<node_type>> color_nodes_; // TODO - use const node_type
	ColorNodeRefList base_;
	ColorNodeRefList extracted_;
};

template<typename T>
Simplifier<T>::Simplifier(T& view)
{
	color_nodes_.reserve(view.vertexes.size() * 2);

	std::unordered_map<node_type const*, int> indexes;
	int i = 0;
	for (const auto& [node, _] : view.vertexes)
	{
		indexes.emplace(node, i++);
		color_nodes_.emplace_back(node);
	}

	for (auto& cn: color_nodes_)
	{
		auto const& edges = view.vertexes[cn.node].edges;
		cn.neighbors.reserve(edges.size());
		std::transform(
			std::cbegin(edges),
			std::cend(edges),
			std::inserter(cn.neighbors, cn.neighbors.end()),
			[this, &indexes](auto const& e) -> color_node_ref
			{ return color_nodes_[indexes[e.link->to]]; });
	}
}

template<typename T>
void Simplifier<T>::simplify(int n)
{
	auto vertexes = init();

	bool done = true;
	int i = 0;
	do
	{
		done = !extract(n, vertexes);
		if (done && n == 5)
		{
			done = !pack(vertexes);
		}
	} while (!done);

	do_base(vertexes);
}

template<typename T>
typename Simplifier<T>::ColorNodeRefList Simplifier<T>::init()
{
	base_.clear();
	extracted_.clear();

	ColorNodeRefList vertexes;
	std::transform(
		std::begin(color_nodes_),
		std::end(color_nodes_),
		std::back_inserter(vertexes),
		[](auto& cn) -> color_node_ref { return cn; });

	return vertexes;
}

template<typename T>
bool Simplifier<T>::extract(int n, ColorNodeRefList& vertexes)
{
	auto extracted = pull(n, vertexes);
	unbind(extracted);
	fold(extracted);
	return !extracted.empty();
}

template<typename T>
bool Simplifier<T>::pack(ColorNodeRefList& vertexes)
{
	if (auto triple = pull_triple(vertexes))
	{
		auto [k, m, n] = *triple;
		auto v = merge(k, m, n);
		vertexes.push_back(v);
		return true;
	}
	return false;
}

template<typename T>
typename Simplifier<T>::ColorNodeRefList
Simplifier<T>::pull(int limit, ColorNodeRefList& vertexes) const
{
	ColorNodeRefList extracted;
	for (auto it = std::begin(vertexes); it != std::end(vertexes);)
	{
		auto& v = it->get();
		if (0 < v.neighbors.size() && v.neighbors.size() < limit)
		{
			extracted.emplace_front(v);
			it = vertexes.erase(it);
		}
		else ++it;
	}

	return extracted;
}

template<typename T>
void Simplifier<T>::unbind(ColorNodeRefList const& vertexes) const
{
	for (auto const& v: vertexes)
	{
		for (auto& n: v.get().neighbors)
		{
			n.get().unbind(v);
		}
	}
}

template<typename T, typename U>
std::optional<std::pair<T, T>>
find_mn(U const& vertexes)
{
	for (auto m = std::cbegin(vertexes); m != std::cend(vertexes); ++m)
	{
		auto n = m;
		std::advance(n, 1);
		for (; n != std::cend(vertexes); ++n)
		{
			if (!m->get().is_neighbor(*n) &&
				m->get().neighbors.size() < 8 &&
				n->get().neighbors.size() < 8)
			{
				return std::make_pair(*m, *n);
			}
		}
	}
	return {};
}

template<typename T>
std::optional<Triple<typename Simplifier<T>::color_node_ref>>
Simplifier<T>::pull_triple(ColorNodeRefList& vertexes) const
{
	for(auto k: vertexes)
	{
		if (k.get().neighbors.size() != 5) continue;
		if (auto mn = find_mn<color_node_ref>(k.get().neighbors))
		{
			vertexes.remove(k);
			vertexes.remove(mn->first);
			vertexes.remove(mn->second);
			return Triple<color_node_ref>{k, mn->first, mn->second};
		}
	}

	return {};
}

template<typename T>
typename Simplifier<T>::color_node_ref Simplifier<T>::merge(
	color_node_ref k,
	color_node_ref m,
	color_node_ref n)
{
	auto& v = color_nodes_.emplace_back(k, m, n);
	k.get().unbind();
	m.get().unbind();
	n.get().unbind();
	v.bind();
	return v;
}

} // namespace

namespace empire
{

template<typename T>
class Colorizer
{
public:
	Colorizer(T& view): view_{view} {}

	void paint_with(bwgui::Color const& color1);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3, bwgui::Color const& color4);
	void paint_with(bwgui::Color const& color1, bwgui::Color const& color2, bwgui::Color const& color3, bwgui::Color const& color4, bwgui::Color const& color5);
	void paint_with(Colors const& colors);
	
private:
	using vertex_type = typename T::vertex_type;
	using node_type = typename T::node_type;

	bool has_colored_neighbour(::ColorNode<node_type> const& v, bwgui::Color const& color) const;
	std::optional<bwgui::Color>
	find_color(::ColorNode<node_type> const& v, Colors const& colors) const;
	bool paint_one_with(::ColorNode<node_type> const& v, Colors const& colors);

	bool has_colored_neighbour(vertex_type const& v, bwgui::Color const& color) const;
	bool paint_one_with(vertex_type& v, Colors const& colors);

	void paint_with(::Simplifier<T> simplifier, Colors const& colors);

	T& view_;
};

template<typename T>
bool Colorizer<T>::has_colored_neighbour(vertex_type const& v, bwgui::Color const& color) const
{
	for (auto const& e: v.edges)
	{
		auto const& neighbour = view_.vertexes[e.link->to];
		if (neighbour.style.background == color)
		{
			return true;
		}
	}
	return false;
}

template<typename T>
bool Colorizer<T>::paint_one_with(vertex_type& v, Colors const& colors)
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

template<typename T>
bool Colorizer<T>::has_colored_neighbour(::ColorNode<node_type> const& v, bwgui::Color const& color) const
{
	for (auto const& n: v.neighbors)
	{
		if (n.get().is_packed()) continue;
		auto const& neighbour = view_.vertexes[n.get().node];
		if (neighbour.style.background == color)
		{
			return true;
		}
	}
	return false;
}

template<typename T>
std::optional<bwgui::Color> 
Colorizer<T>::find_color(::ColorNode<node_type> const& v, Colors const& colors) const
{
	for (auto const& color: colors)
	{
		if (!has_colored_neighbour(v, color))
		{
			return color;
		}
	}
	return {};
}

template<typename T>
bool Colorizer<T>::paint_one_with(::ColorNode<node_type> const& v, Colors const& colors)
{
	if (v.is_packed())
	{
		if (auto color = find_color(v, colors))
		{
			auto [k, m, n] = v.unpack();
			view_.vertexes[m.get().node].style.background = *color;
			view_.vertexes[n.get().node].style.background = *color;
			return paint_one_with(view_.vertexes[k.get().node], colors);
		}
		return false;
	}
	auto& vertex = view_.vertexes[v.node];
	return paint_one_with(vertex, colors);
}

template<typename T>
void Colorizer<T>::paint_with(::Simplifier<T> simplifier, Colors const& colors)
{
	simplifier.simplify(colors.size());

	for (auto v: simplifier.base())
	{
		auto const ok = paint_one_with(v.get(), colors);
		if (!ok) return;
	}

	for (auto v: simplifier.extracted())
	{
		auto const ok = paint_one_with(v.get(), colors);
		if (!ok) return;
	}
}

template<typename T>
void Colorizer<T>::paint_with(bwgui::Color const& color1)
{
	for (auto& [n, v]: view_.vertexes) v.style.background = color1;
}

template<typename T>
void Colorizer<T>::paint_with(bwgui::Color const& color1, bwgui::Color const& color2)
{
	if (view_.vertexes.empty()) return;

	auto& [n, v] = *view_.vertexes.begin();
	v.style.background = color1;
	print_colored(v);

	std::queue<vertex_type const*> colored;
	colored.push(&v);
	
	while (!colored.empty())
	{
		auto v = colored.front();
		colored.pop();
		
		auto const& color = v->style.background == color1 ? color2 : color1;

		for (auto const& e: v->edges)
		{
			auto& to = view_.vertexes[e.link->to];
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

template<typename T>
void Colorizer<T>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3)
{
	paint_with(::Simplifier{view_}, {color1, color2, color3});
}

template<typename T>
void Colorizer<T>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3,
	bwgui::Color const& color4)
{
	paint_with(::Simplifier{view_}, {color1, color2, color3, color4});
}

template<typename T>
void Colorizer<T>::paint_with(
	bwgui::Color const& color1,
	bwgui::Color const& color2,
	bwgui::Color const& color3,
	bwgui::Color const& color4,
	bwgui::Color const& color5)
{
	paint_with(::Simplifier{view_}, {color1, color2, color3, color4, color5});
}

template<typename T>
void Colorizer<T>::paint_with(Colors const& colors)
{
	for (auto& [n, v]: view_.vertexes)
	{
		auto const ok = paint_one_with(v, colors);
		if (!ok) return;
	}
}

} // namespace empire

#endif // _COLORIZER_H_
