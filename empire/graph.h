#ifndef _GRAPH_H_
#define _GRAPH_H_

#include <algorithm>
#include <functional>
#include <limits>
#include <list>
#include <map>
#include <memory>
#include <stack>
#include <queue>
#include <unordered_map>
#include <iostream>

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
	U mark{0};
};

template<typename T, typename U = int>
struct Link
{
	Node<T, U>* from;
	Node<T, U>* to;
	U cost;
};

template<typename T, typename U = int>
using NodePtr = std::unique_ptr<Node<T, U>>;

template<typename T, typename U = int>
using Nodes = std::vector<NodePtr<T, U>>;

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
void mark_traverse(NodePtr<T, U> const& node, TraverseFunctor func)
{
	Node<T, U> before{-1, {}};
	Link<T, U> start{&before, node.get(), 0};

	std::list<Link<T,U>*const> links;
	links.push_back(&start);
	
	while (!links.empty())
	{
		auto found = std::min_element(std::begin(links), std::end(links), 
			[](auto const lhs, auto const rhs) {
				return (lhs->from->mark + lhs->cost) < (rhs->from->mark + rhs->cost);
			});
		if (found != std::end(links))
		{
			auto cur = *found;
			if (cur->to->visited)
			{
				links.erase(found);
				continue;
			}
			cur->to->visited = true;
			cur->to->mark = cur->from->mark + cur->cost;
		
			func(cur->from->index, cur->to->index);
			for (auto& l: cur->to->links)
			{
				if (!l.to->visited) links.push_back(&l);
			}
		}
	}
}

template<typename T, typename U = int>
void remark_traverse(NodePtr<T, U> const& node, TraverseFunctor func)
{
	Node<T, U> before{-1, {}};
	Link<T, U> start{&before, node.get(), 0};

	std::unordered_map<Node<T, U>*, Node<T, U>*> to_from;
	
	std::queue<Link<T, U>*> links;
	links.push(&start);
	
	while (!links.empty())
	{
		auto cur = links.front();
		links.pop();
		auto remark = cur->from->mark + cur->cost;
		if (remark < cur->to->mark)
		{
			cur->to->mark = remark;
			to_from[cur->to] = cur->from;
			for (auto& l: cur->to->links) links.push(&l);
		}
	}

	std::vector<typename decltype(to_from)::value_type const*> tree;
	tree.reserve(to_from.size());
	std::transform(
		std::begin(to_from),
		std::end(to_from),
		std::back_inserter(tree),
		[](auto const& p){ return &p; });
	std::sort(std::begin(tree), std::end(tree),
		[](auto const& lhs, auto const& rhs) {
			 return lhs->first->mark < rhs->first->mark;
	});
	for (auto p: tree)
		func(p->second->index, p->first->index);
}

namespace
{
std::vector<int> read_path(std::unordered_map<int, int> const& steps, int from, int to)
{
	std::vector<int> path{to};
	while (path.back() != from)
	{
		if (auto found = steps.find(path.back()); found != std::end(steps))
		{
			path.push_back(found->second);
		}
		else
		{
			return {};
		}
	}
	std::reverse(path.begin(), path.end());
	return path;
}
} // namespace 

template<typename T, typename U = int>
using LoopFunctor = std::function<void(NodePtr<T, U> const&, TraverseFunctor)>;

template<typename T, typename U = int>
struct Graph
{
	NodePtr<T, U> const& operator[](int i) const { return nodes[i]; }
	void traverse(int i, TraverseFunctor func, LoopFunctor<T,U> loop = width_traverse<T, U>)
	{
		for (auto const& n: nodes)
		{
			n->visited = false;
			n->mark = std::numeric_limits<int>::max();
		}
		loop(nodes[i], func);
	}
	std::vector<int> find_path(int from, int to, LoopFunctor<T,U> loop = width_traverse<T, U>)
	{
		std::unordered_map<int, int> steps;
		traverse(from, [&steps](int from, int to) { steps.emplace(to, from); }, loop);
		return read_path(steps, from, to);
	}

	Nodes<T, U> nodes;
};

template<typename T = int>
struct MetaLink
{
	size_t from;
	size_t to;
	T cost;
};

template<typename U = int>
using Comparator = std::function<bool(U const&, U const&)>;

template<typename T, typename U = int>
Graph<T, U> make_graph(
	std::vector<T>& values,
	std::vector<MetaLink<U>> const& links,
	Comparator<U> comp = std::less<U>{})
{
	Graph<T, U> graph;
	graph.nodes.reserve(values.size());
	for (auto i = 0; i < values.size(); ++i)
	{
		graph.nodes.push_back(make_node<T,U>(i, std::forward<T>(values[i])));
	}
	for (auto const& l : links)
	{
		graph[l.from]->links.push_back({graph[l.from].get(), graph[l.to].get(), l.cost});
	}
	for (auto& n: graph.nodes)
	{
		std::sort(std::begin(n->links), std::end(n->links),
			[comp](auto const& lhs, auto const& rhs) { return comp(lhs.cost, rhs.cost); });
	}
	return graph;
}

} // namespace empire

#endif // _GRAPH_H_
