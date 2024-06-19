#ifndef _TOPOLOGIC_H_
#define _TOPOLOGIC_H_

#include "graph.h"

namespace empire
{

template<typename T, typename U = int>
std::vector<int> topologic_sort(Graph<T, U> const& graph)
{
	std::vector<int> dependencies(graph.nodes.size(), 0);
	for (auto const& n: graph.nodes)
		for (auto const& l: n->links)
			++dependencies[l.to->index];

	std::queue<Node<T, U> const * const> ready;
	for (auto const& n: graph.nodes)
		if (dependencies[n->index] == 0) ready.push(n.get());
	 
	std::vector<int> ordering;
	while (!ready.empty())
	{
		auto n = ready.front();
		ready.pop();
		ordering.push_back(n->index);
		for (auto const& l: n->links)
		{
			--dependencies[l.to->index];
			if (dependencies[l.to->index] == 0) ready.push(l.to);
		}
	}
	if (std::any_of(
		std::begin(dependencies),
		std::end(dependencies),
		[](auto c) { return c > 0; }))
	{
		return {};
	}
	return ordering;
}

} // namespace empire

#endif // _TOPOLOGIC_H_
