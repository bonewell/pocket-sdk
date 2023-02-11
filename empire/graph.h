#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <functional>
#include <limits>
#include <memory>
#include <stack>
#include <queue>

namespace empire
{

template<typename T, typename U> struct Link;

template<typename T, typename U = int>
struct Node
{
	Node(int i, T&& v) : index{i}, value{std::forward<T>(v)} {}

	int index;
	T value;
	std::vector<Link<T, U>> links;

	bool visited{false};
};

template<typename T, typename U = int>
struct Link
{
	Node<T, U>*const from;
	Node<T, U>*const to;
	U cost;
};

template<typename T, typename U = int>
using NodePtr = std::unique_ptr<Node<T, U>>;

template<typename T, typename U = int>
NodePtr<T, U> make_node(int i, T&& value)
{
	return std::make_unique<Node<T, U>>(i, std::forward<T>(value));
}

using TraverseFunctor = std::function<void(int /*from*/, int /*to*/)>;

template<typename T, typename U = int>
void width_traverse(NodePtr<T, U> const& node, TraverseFunctor func)
{
	node->visited = true;
	
	std::queue<Link<T, U>*const> links;
	Node<T, U> before{-1, {}};
	Link<T, U> start{&before, node.get()};
	links.push(&start);
	
	while (!links.empty())
	{
		auto cur = links.front();
		links.pop();
		func(cur->from->index, cur->to->index);
		for (auto& l: cur->to->links)
		{
			if (!l.to->visited)
			{
				l.to->visited = true;
				links.push(&l);
			}
		}
	}
}

template<typename T, typename U = int>
void depth_traverse(NodePtr<T, U> const& node, TraverseFunctor func)
{
	node->visited = true;
	
	std::stack<Link<T, U>*const> links;
	Node<T, U> before{-1, {}};
	Link<T, U> start{&before, node.get()};
	links.push(&start);
	
	while (!links.empty())
	{
		auto cur = links.top();
		links.pop();
		func(cur->from->index, cur->to->index);
		for (auto& l: cur->to->links)
		{
			if (!l.to->visited)
			{
				l.to->visited = true;
				links.push(&l);
			}
		}
	}
}

template<typename T, typename U = int>
struct Graph
{
	NodePtr<T, U> const& operator[](int i) const { return nodes[i]; }
	void width_loop(int i, TraverseFunctor func)
	{
		for (auto const& n: nodes) n->visited = false;
		width_traverse<T, U>(nodes[i], func);
	}
	void depth_loop(int i, TraverseFunctor func)
	{
		for (auto const& n: nodes) n->visited = false;
		depth_traverse<T, U>(nodes[i], func);
	}
	std::vector<NodePtr<T, U>> nodes;
};

template<typename T = int>
struct MetaLink
{
	size_t from;
	size_t to;
	T cost;
};

template<typename T, typename U = int>
Graph<T, U> make_graph(std::vector<T>& values, std::vector<MetaLink<U>> const& links = {})
{
	Graph<T, U> graph;
	graph.nodes.reserve(values.size());
	for (auto i = 0; i < values.size(); ++i)
	{
		graph.nodes.push_back(make_node(i, std::forward<T>(values[i])));
	}
	for (auto const& l : links)
	{
		graph[l.from]->links.push_back({graph[l.from].get(), graph[l.to].get(), l.cost});
	}
	return graph;
}

} // namespace empire

#endif // _GRAPH_H_
