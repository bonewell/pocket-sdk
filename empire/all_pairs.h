#ifndef _ALL_PAIRS_H_
#define _ALL_PAIRS_H_

#include "graph.h"

#include <numeric>

namespace empire
{
template<typename T>
T inf = std::numeric_limits<T>::max();

template<typename T, typename U = int>
class AllPairs
{
public:
	explicit AllPairs(Graph<T,U> const& graph);
	std::vector<int> find_path(int from, int to) const;

private:
	void init();
	void precalc();

	Nodes<T,U> const& nodes;

	std::vector<std::vector<U>> distance_;
	std::vector<std::vector<int>> via_;
};

template<typename T, typename U>
AllPairs<T, U>::AllPairs(Graph<T,U> const& graph)
	: nodes{graph.nodes},
	  distance_(nodes.size(), std::vector<U>(nodes.size(), inf<U>)),
	  via_(nodes.size(), std::vector<int>(nodes.size(), -1))
{
	init();
	precalc();
}

template<typename T, typename U>
void AllPairs<T, U>::init()
{
	for(auto i = 0; i < nodes.size(); ++i)
	{
		distance_[i][i] = U{};
		via_[i][i] = i;
		for (auto l: nodes[i]->links)
		{
			const auto j = l.to->index;
			distance_[i][j] = l.cost;
			via_[i][j] = j;
		}
	}
}

template<typename T, typename U>
void AllPairs<T, U>::precalc()
{
	for(auto via = 0; via < nodes.size(); ++via)
	{
		for (auto from = 0; from < nodes.size(); ++from)
		{
			for (auto to = 0; to < nodes.size(); ++to)
			{
				if (distance_[from][via] == inf<U>) continue;
				if (distance_[via][to] == inf<U>) continue;
				auto const distance = distance_[from][via] + distance_[via][to];
				if (distance < distance_[from][to])
				{
					distance_[from][to] = distance;
					via_[from][to] = via;
				}
			}
		}
	}
}

template<typename T, typename U>
std::vector<int> AllPairs<T, U>::find_path(int from, int to) const
{
	if (distance_[from][to] == inf<U>) return {};
	auto const via = via_[from][to];
	if (via == to) return {from, to};
	auto head = find_path(from, via);
	auto tail = find_path(via, to);
	head.insert(std::end(head), std::next(std::begin(tail)), std::end(tail));
	return head;
}

} // namespace empire

#endif // _ALL_PAIRS_H_
