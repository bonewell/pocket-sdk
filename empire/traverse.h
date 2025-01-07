#ifndef _TRAVERSE_H_
#define _TRAVERSE_H_

#include <list>
#include <stack>
#include <queue>
#include <unordered_map>
#include <unordered_set>

namespace empire
{

template<typename T, typename F>
void depth_traverse(T* node, F func)
{
	using node_type = T;
	using link_type = typename T::link_type;
	
	std::unordered_set<node_type*> visited;
	std::stack<link_type*> links;
	
	auto visit = [&links, &visited](auto* node)
	{
		for (auto& l: node->links)
		{
			if (visited.count(l.to) == 0)
			{
				visited.insert(l.to);
				links.push(&l);
			}
		}
	};

	visited.insert(node);
	visit(node);
	while (!links.empty())
	{
		auto cur = links.top();
		links.pop();
		func(cur);
		visit(cur->to);
	}
}

template<typename T, typename F>
void width_traverse(T* node, F func)
{
	using node_type = T;
	using link_type = typename T::link_type;

	std::unordered_set<node_type*> visited;
	std::queue<link_type*> links;

	auto visit = [&links, &visited](auto* node)
	{
		for (auto& l: node->links)
		{
			if (visited.count(l.to) == 0)
			{
				visited.insert(l.to);
				links.push(&l);
			}
		}
	};

	visited.insert(node);
	visit(node);
	while (!links.empty())
	{
		auto cur = links.front();
		links.pop();
		func(cur);
		visit(cur->to);
	}
}

template<typename T, typename F>
void mark_traverse(T* node, F func)
{
	using node_type = T;
	using link_type = typename T::link_type;
	using cost_type = typename T::cost_type;

	std::unordered_set<node_type*> visited;
	std::unordered_map<node_type*, cost_type> marks;
	std::list<link_type*> links;
	
	auto visit = [&links, &visited](auto* node)
	{
		for (auto& l: node->links)
		{
			if (visited.count(l.to) == 0)
			{
				links.push_back(&l);
			}
		}
	};

	auto top = [&links, &marks, &visited] () -> link_type*
	{
		auto found = std::min_element(std::begin(links), std::end(links), 
			[&marks](auto const lhs, auto const rhs) {
				return (marks[lhs->from] + lhs->cost) < (marks[rhs->from] + rhs->cost);
			});
		if (found != std::end(links))
		{
			if (visited.count((*found)->to) == 0) return *found;
			links.erase(found);
		}
		return nullptr;
	};

	visited.insert(node);
	marks[node];
	visit(node);
	while (!links.empty())
	{
		if (auto cur = top())
		{
			visited.insert(cur->to);
			marks[cur->to] = marks[cur->from] + cur->cost;
			func(cur);
			visit(cur->to);
		}
	}
}

template<typename T, typename F>
void remark_traverse(T* node, F func)
{
	using node_type = T;
	using link_type = typename T::link_type;
	using cost_type = typename T::cost_type;

	std::unordered_map<node_type*, cost_type> marks;
	std::queue<link_type*> links;

	auto visit = [&links](auto* node)
	{
		for (auto& l: node->links) links.push(&l);
	};
	
	std::unordered_map<node_type*, link_type*> to_from;

	marks[node];
	visit(node);
	while (!links.empty())
	{
		auto cur = links.front();
		links.pop();
		auto remark = marks[cur->from] + cur->cost;
		if (marks.count(cur->to) == 0 || remark < marks[cur->to])
		{
			marks[cur->to] = remark;
			to_from[cur->to] = cur;
			visit(cur->to);
		}
	}

	std::vector<link_type*> tree;
	tree.reserve(to_from.size());
	std::transform(
		std::begin(to_from),
		std::end(to_from),
		std::back_inserter(tree),
		[](auto const& p){ return p.second; });
	std::sort(std::begin(tree), std::end(tree),
		[&marks](auto const& lhs, auto const& rhs) {
			 return marks[lhs->to] < marks[rhs->to];
	});

	for (auto cur: tree) func(cur);
}

} // namespace empire

#endif // _TRAVERSE_H_
