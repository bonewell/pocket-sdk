#ifndef _TREE_H_
#define _TREE_H_

#include <algorithm>
#include <functional>
#include <memory>
#include <queue>
#include <utility>
#include <vector>

namespace taiga
{

struct Thread {};

template<typename T, typename U = Thread> struct Node;

template<typename T, typename U = Thread>
using Link = std::unique_ptr<Node<T, U>>;

template<typename T, typename U = Thread> using Tree = Link<T, U>;

template<typename T, typename U>
struct Node
{
	Node(T&& v, U&& t) :
		value{std::forward<T>(v)},
		thread{std::forward<U>(t)} {}

	T value;
	Link<T, U> left;
	Link<T, U> right;
	U thread;
};

template<typename T, typename U = Thread>
Link<T, U> make_node(T&& value, U&& thread = {})
{
	return std::make_unique<Node<T, U>>(std::forward<T>(value), std::forward<U>(thread));
}

template<typename T, typename U = Thread>
void add_node(Link<T, U>& link, T&& value)
{
	if (link)
	{
		if (value < link->value)
			add_node(link->left, std::forward<T>(value));
		else
			add_node(link->right, std::forward<T>(value));
	}
	else link = make_node(std::forward<T>(value));
}

template<typename T, typename U = Thread>
Link<T, U> make_tree(std::vector<T> const& values = {})
{
	Link<T, U> tree;
	for (auto v: values)
	{
		add_node(tree, std::forward<T>(v));
	}
	return tree;
}

template<typename T, typename U = Thread>
int depth_tree(Link<T, U> const& link, int level = 0)
{
	auto left_level = link->left ? depth_tree(link->left, level + 1) : level;
	auto right_level = link->right ? depth_tree(link->right, level + 1) : level;
	return std::max(left_level, right_level);
}

template<typename T>
using TraverseFunctor = std::function<void(T const&)>;

template<typename T, typename U = Thread>
void forward_traverse(Link<T, U> const& link, TraverseFunctor<T> func)
{
	func(link->value);
	if (link->left) forward_traverse(link->left, func);
	if (link->right) forward_traverse(link->right, func);
}

template<typename T, typename U = Thread>
void traverse(Link<T, U> const& link, TraverseFunctor<T> func)
{
	forward_traverse<T, U>(link, func);
}

template<typename T, typename U = Thread>
void symmetric_traverse(Link<T, U> const& link, TraverseFunctor<T> func)
{
	if (link->left) symmetric_traverse(link->left, func);
	func(link->value);
	if (link->right) symmetric_traverse(link->right, func);
}

template<typename T, typename U = Thread>
void backward_traverse(Link<T, U> const& link, TraverseFunctor<T> func)
{
	if (link->left) backward_traverse(link->left, func);
	if (link->right) backward_traverse(link->right, func);
	func(link->value);
}

template<typename T, typename U = Thread>
void level_traverse(Link<T, U> const& link, TraverseFunctor<T> func)
{
	std::queue<Node<T, U> const*> q;
	q.push(link.get());
	while (!q.empty())
	{
		auto n = q.front();
		q.pop();
		func(n->value);
		if (n->left) q.push(n->left.get());
		if (n->right) q.push(n->right.get());
	}
}

template<typename T>
using ModifyFunctor = std::function<void(T&)>;

template<typename T, typename U = Thread>
void apply(Link<T, U>& link, ModifyFunctor<T> func)
{
	func(link->value);
	if (link->left) apply(link->left, func);
	if (link->right) apply(link->right, func);
}

} // namespace taiga
#endif // _TREE_H_
