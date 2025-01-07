#ifndef _ALL_PAIRS_H_
#define _ALL_PAIRS_H_

#include "graph.h"

#include <numeric>

namespace empire
{
template<typename T>
T inf = std::numeric_limits<T>::max();

template<typename T>
class AllPairs
{
public:
	using node_type = typename T::node_type;
	using cost_type = typename T::link_type::cost_type;

	explicit AllPairs(T const& graph): graph_{graph}
	{
		init();
		precalc();
	}

	std::vector<node_type const*> find_path(node_type const* from, node_type const* to) const
	{
		if (distance_.at(from).at(to) == inf<cost_type>) return {};
		auto const via = via_.at(from).at(to);
		if (via == to) return {from, to};
		auto head = find_path(from, via);
		auto tail = find_path(via, to);
		head.insert(std::end(head), std::next(std::begin(tail)), std::end(tail));
		return head;
	}

private:
	void init();
	void precalc();

	T const& graph_;

	using Distance = std::unordered_map<node_type const*,
		std::unordered_map<node_type const*, cost_type>>;
	Distance distance_;

	using Via = std::unordered_map<node_type const*,
		std::unordered_map<node_type const*, node_type const*>>;
	Via via_;
};

template<typename T>
void AllPairs<T>::init()
{
	for(auto const& nfrom: graph_)
	{
		auto from = nfrom.get();
		for (auto const& nto: graph_)
		{
			auto to = nto.get();
			distance_[from][to] = inf<cost_type>;
		}
		distance_[from][from] = cost_type{};
		via_[from][from] = from;
		for (auto l: from->links)
		{
			const auto to = l.to;
			distance_[from][to] = l.cost;
			via_[from][to] = to;
		}
	}
}

template<typename T>
void AllPairs<T>::precalc()
{
	for(auto const& nvia: graph_)
	{
		auto via = nvia.get();
		for (auto const& nfrom: graph_)
		{
			auto from = nfrom.get();
			for (auto const& nto: graph_)
			{
				auto to = nto.get();
				if (distance_[from][via] == inf<cost_type>) continue;
				if (distance_[via][to] == inf<cost_type>) continue;
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

} // namespace empire

#endif // _ALL_PAIRS_H_
