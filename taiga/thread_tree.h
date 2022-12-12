#ifndef _THREAD_TREE_H_
#define _THREAD_TREE_H_

#include "tree.h"

namespace taiga
{

template<typename T> struct Threads;

template<typename T>
using ThreadLink = Link<T, Threads<T>>;

template<typename T>
using NodePtr = Node<T, Threads<T>> const*;

template<typename T>
struct Threads
{
	NodePtr<T> left{nullptr};
	NodePtr<T> right{nullptr}; 
};

template<typename T>
ThreadLink<T> make_thread_node(
	T&& value,
	NodePtr<T> left = nullptr,
	NodePtr<T> right = nullptr)
{
	return make_node<T, Threads<T>>(std::forward<T>(value), {left, right});
}

template<typename T>
using ThreadTree = ThreadLink<T>;

template<typename T>
void add_link_thread_node(ThreadLink<T>& link, T&& value)
{
	if (value < link->value)
	{
		if (link->left) add_link_thread_node(link->left, std::forward<T>(value));
		else
		{
			link->left = make_thread_node(
				std::forward<T>(value),
				link->thread.left,
				link.get());
			link->thread.left = nullptr;
		}
	}
	else
	{
		if (link->right) add_link_thread_node(link->right, std::forward<T>(value));
		else
		{
			link->right = make_thread_node(
				std::forward<T>(value),
				link.get(),
				link->thread.right);
			link->thread.right = nullptr;
		}
	}
}

template<typename T>
void add_thread_node(ThreadLink<T>& tree, T&& value)
{
	if (tree) add_link_thread_node(tree, std::forward<T>(value));
	else tree = make_thread_node(std::forward<T>(value));
}

template<typename T>
ThreadLink<T> make_thread_tree(std::vector<T> const& values = {})
{
	ThreadLink<T> tree;
	for (auto v: values)
	{
		add_thread_node<T>(tree, std::forward<T>(v));
	}
	return tree;
}

template<typename T>
void symmetric_forward_traverse(ThreadLink<T> const& link, TraverseFunctor<T> func)
{
	NodePtr<T> node = link.get();
	auto via_branch{true};
	while (node)
	{
		if (via_branch)
		{
			while (node->left) node = node->left.get();
		}
		func(node->value);
		if (node->right)
		{
			node = node->right.get();
			via_branch = true;
		}
		else
		{
			node = node->thread.right;
			via_branch = false;
		}
	}
}

template<typename T>
void symmetric_backward_traverse(ThreadLink<T> const& link, TraverseFunctor<T> func)
{
	NodePtr<T> node = link.get();
	auto via_branch{true};
	while (node)
	{
		if (via_branch)
		{
			while (node->right) node = node->right.get();
		}
		func(node->value);
		if (node->left)
		{
			node = node->left.get();
			via_branch = true;
		}
		else
		{
			node = node->thread.left;
			via_branch = false;
		}
	}
}

} // namespace taigs

#endif // _THREAD_TREE_H_
