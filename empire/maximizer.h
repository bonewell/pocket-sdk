#ifndef _MAXIMAZER_H_
#define _MAXIMAZER_H_

#include "graph.h"

namespace empire
{

struct Pipe
{
	int volume;	
	int flow{0};
	Pipe* residual{nullptr};
 
	int residual_flow() const { return volume - flow; }
	Pipe residual_pipe() { return {volume, residual_flow(), this}; }

	operator std::string() const
	{ return std::to_string(flow) + '/' + std::to_string(volume); }
};

Pipe operator+(Pipe const& lhs, Pipe const& rhs)
{ return {lhs.volume + rhs.volume, lhs.flow + rhs.flow}; }

bool operator<(Pipe const& lhs, Pipe const& rhs)
{ return lhs.residual_flow() < rhs.residual_flow(); }

namespace {

template<typename T>
struct Network
{
	using link_type = typename T::link_type;

	T graph;
	std::vector<link_type> filled;
};

template<typename T>
int minimal_residual(T const& path)
{
	auto min = std::min_element(std::begin(path), std::end(path),
		[](auto const& lhs, auto const& rhs)
			{ return lhs->cost.residual_flow() < rhs->cost.residual_flow(); });
	return min != std::end(path) ? (*min)->cost.residual_flow() : 0;
}

template<typename T>
void improve_stream(T& path, int additional_flow)
{
	for (auto& p: path)
	{
		if (p->cost.residual)
		{
			p->cost.residual->flow -= additional_flow;
		}
		else
		{
			p->cost.flow += additional_flow;
		}
	}
}

template<typename T>
Network<T> add_residual_network(T&& graph)
{
	Network<T> net{std::forward<T>(graph)};
	for (auto const& n: net.graph)
	{
		for (auto& l: n->links)
		{
			auto border = std::partition(std::begin(n->links), std::end(n->links),
				[](auto const& l){ return l.cost.residual_flow() != 0; });
			std::move(border, std::end(n->links), std::back_inserter(net.filled));
			n->links.erase(border, std::end(n->links));
		}
	}
	
	auto add_residual_pipe = [](auto& l){
		auto pipe = l.cost.residual_pipe();
		if (pipe.residual_flow() != 0)
		{
			l.to->links.emplace_back(l.to, l.from, std::move(pipe));
		}
	};

	for (auto const& n: net.graph)
	{
		for (auto& l: n->links)
		{
			if (!l.cost.residual)
			{
				add_residual_pipe(l);
			}
		}
	}
	for (auto& l: net.filled)
	{
		add_residual_pipe(l);
	}
	return net;
}

} // namespace

template<typename T>
T remove_residual_network(Network<T>&& net)
{
	for (auto const& n: net.graph)
	{
		n->links.erase(
			std::remove_if(std::begin(n->links), std::end(n->links),
				[](auto const& l){ return l.cost.residual; }),
			std::end(n->links));
	}
	for (auto& l: net.filled)
	{
		l.from->links.push_back(std::move(l));
	}
	return std::move(net.graph);
}

template<typename T>
T maximize_stream(T&& graph)
{
	auto net = add_residual_network(std::forward<T>(graph));
	auto path = net.graph.find_path(std::begin(net.graph)->get(),
									std::rbegin(net.graph)->get());

	while (!path.empty())
	{
		auto min = minimal_residual(path);
		improve_stream(path, min);
		net = add_residual_network(remove_residual_network(std::move(net)));
	
		path = net.graph.find_path(std::begin(net.graph)->get(),
								   std::rbegin(net.graph)->get());
	}
	return remove_residual_network(std::move(net));
}

template<typename T>
T cut_maximized_stream(T&& stream)
{
	auto net = add_residual_network(std::move(stream));
	std::unordered_set<typename T::node_type*> a;
	net.graph.traverse(net.graph.begin()->get(),
		[&a](auto l) {
			a.insert(l->from);
			a.insert(l->to);
		});
	auto graph = remove_residual_network(std::move(net));
	for (auto n: a)
	{
		n->links.erase(
			std::remove_if(std::begin(n->links), std::end(n->links),
				[&a](auto const& l){ return a.count(l.to) == 0; }),
			std::end(n->links));
	}
	return graph;
}

} // namespace empire

#endif // _MAXIMAZER_H_
