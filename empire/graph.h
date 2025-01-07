#ifndef _GRAPH_H_
#define _GRAPH_H_

#include "traverse.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <unordered_map>
#include <iostream>
#include <vector>

namespace empire
{

template<typename T, typename U = int>
struct Link
{
	using node_type = T;
	using cost_type = U;
	using link_type = Link<T, U>;

	Link(node_type* from, node_type* to, cost_type cost):
		from{from}, to{to}, cost{std::move(cost)} {}
	
	node_type* from;
	node_type* to;
	cost_type cost;
};
		
template<typename T, typename U = int>
struct Node
{
	using value_type = T;
	using cost_type = U;
	using node_type = Node<T, U>;
	using link_type = Link<node_type, cost_type>;

	explicit Node(value_type&& v) : value{std::forward<value_type>(v)} {}

	value_type value;
	std::vector<link_type> links;
};

template<typename T, typename U = int>
using NodePtr = std::unique_ptr<Node<T, U>>;

enum class Traverse { Width, Depth, Mark, Remark };

namespace
{
	
template<typename T, typename U = int>
NodePtr<T, U> make_node(T&& value)
{
	return std::make_unique<Node<T, U>>(std::forward<T>(value));
}

template<typename N, typename F>
void loop(N* node, F&& func, Traverse type)
{
	switch(type) {
		case Traverse::Depth: depth_traverse(node, std::forward<F>(func)); break;
		case Traverse::Width: width_traverse(node, std::forward<F>(func)); break;
		case Traverse::Mark: mark_traverse(node, std::forward<F>(func)); break;
		case Traverse::Remark: remark_traverse(node, std::forward<F>(func)); break;
		default: break;
	}
}

template<typename T>
std::vector<typename T::link_type*>
read_path(T* from, T* to, std::unordered_map<T*, typename T::link_type*> steps)
{
	auto cur = to;
	std::vector<typename T::link_type*> path;
	do
	{
		if (auto found = steps.find(cur); found != std::end(steps))
		{
			cur = found->second->from;
			path.push_back(found->second);
		}
		else return {};
	}
	while (path.back()->from != from);

	std::reverse(path.begin(), path.end());
	return path;
}

} // namespace


template<typename T = int>
struct MetaLink
{
	size_t from;
	size_t to;
	T cost;
};

template<typename T, typename U = int>
struct Graph
{
	using node_type = Node<T, U>;
	using link_type = Link<node_type, U>;
	using node_iterator = typename std::vector<NodePtr<T, U>>::const_iterator;
	using reverse_node_iterator = typename std::vector<NodePtr<T, U>>::const_reverse_iterator;
	
	Graph(std::vector<NodePtr<T, U>> nodes): nodes_{std::move(nodes)} {}

	size_t size() const { return nodes_.size(); }
	node_type* operator[](int i) const { return nodes_[i].get(); }
	node_iterator begin() const { return nodes_.begin(); }
	node_iterator end() const { return nodes_.end(); }
	reverse_node_iterator rbegin() const { return nodes_.rbegin(); }
	reverse_node_iterator rend() const { return nodes_.rend(); }

	template<typename UnaryFunc>
	void traverse(node_type* node, UnaryFunc&& func, Traverse type = Traverse::Width)
	{
		loop(node, std::forward<UnaryFunc>(func), type);
	}

	std::vector<link_type*>
	find_path(node_type* from, node_type* to, Traverse type = Traverse::Width)
	{
		std::unordered_map<node_type*, link_type*> steps;
		traverse(
			from,
			[&steps](auto* link) { steps.emplace(link->to, link); },
			type);
		return read_path(from, to, std::move(steps));
	}

private:
	std::vector<NodePtr<T, U>> nodes_;
};

template<typename U>
using Comparator = std::function<bool(U const&, U const&)>;

template<typename T, typename U = int>
Graph<T, U> make_graph(
	std::vector<T> values,
	std::vector<MetaLink<U>> links)
{
	std::vector<NodePtr<T, U>> nodes;
	nodes.reserve(values.size());
	std::transform(std::begin(values), std::end(values), std::back_inserter(nodes),
		[](auto& v) { return make_node<T, U>(std::move(v)); });

	for (auto& l : links)
	{
		nodes[l.from]->links.push_back({nodes[l.from].get(),
										nodes[l.to].get(),
										std::move(l.cost)});
	}

	return {std::move(nodes)};
}

} // namespace empire

#endif // _GRAPH_H_
