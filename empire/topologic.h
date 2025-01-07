#ifndef _TOPOLOGIC_H_
#define _TOPOLOGIC_H_

#include "graph.h"

namespace empire
{

template<typename T>
std::vector<typename T::node_type const*> topologic_sort(T const& graph)
{
	using node_type = typename T::node_type;

	std::unordered_map<node_type const*, int> dependencies;
	dependencies.reserve(graph.size());

	for (auto const& n: graph)
		for (auto const& l: n->links)
			++dependencies[l.to];

	std::queue<node_type const*> ready;
	for (auto const& n: graph)
		if (dependencies[n.get()] == 0) ready.push(n.get());
	 
	std::vector<node_type const*> ordering;
	while (!ready.empty())
	{
		auto n = ready.front();
		ready.pop();
		ordering.push_back(n);
		for (auto const& l: n->links)
		{
			--dependencies[l.to];
			if (dependencies[l.to] == 0) ready.push(l.to);
		}
	}
	if (std::any_of(
		std::begin(dependencies),
		std::end(dependencies),
		[](auto const& p) { return p.second > 0; }))
	{
		return {};
	}
	return ordering;
}

} // namespace empire

#endif // _TOPOLOGIC_H_
