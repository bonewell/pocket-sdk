#include <iostream>

#include "taiga/prefix_tree.h"

template<typename T>
void print_debug(taiga::PrefixLink<T> const& link)
{
	if (!link) return;
	std::cout << link->key << '=';
	if (link->value)
		std::cout << *link->value;
	else
		std::cout << "null";
	std::cout << '(';
	for (auto const& [k, c]: link->children)
	{
		std::cout << k << ':';
		print_debug(c);
	}
	std::cout << ')';
}

template<typename T>
void traverse(taiga::PrefixLink<T> const& link, int level = 0)
{
	if (!link) return;

	if (link->value)
	{
		for (int i = 0; i < level; ++i) std::cout << '\t';
		std::cout << link->key << '=' << *link->value << '\n';
	}
	for (auto const& [k, c]: link->children)
	{
		for (int i = 0; i < level; ++i) std::cout << '\t';
		std::cout << k << '\n';
		traverse(c, level + 1);
	}
}

int main()
{
	auto tree = taiga::make_prefix_tree<int>({
		{"APPLE", 10},
		{"APP", 20},
		{"BEAR", 30},
		{"ANT", 40},
		{"BAT", 50},
		{"APE", 60}
	});

	std::cout << "ANT = " << taiga::find(tree, "ANT") << "\n\n";

	print_debug(tree);
	std::cout << '\n';
	traverse(tree);

	return 0;
}
