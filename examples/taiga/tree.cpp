#include <iostream>
#include <queue>

#include "taiga/tree.h"
#include "taiga/thread_tree.h"

template<typename T>
using FunctionIndent = std::function<void(T const&, int)>;

template<typename T>
void forward_traverse_with_indent(taiga::Link<T> const& link, FunctionIndent<T> func, int level = 0)
{
	func(link->value, level);
	if (link->left) forward_traverse_with_indent(link->left, func, level + 1);
	if (link->right) forward_traverse_with_indent(link->right, func, level + 1);
}

int main()
{
	auto tree = taiga::make_tree<char>({'E', 'B', 'A', 'D', 'C', 'F', 'I', 'G', 'H', 'J'});

	auto print_value = [] (auto const& v) { std::cout << v << ' '; };
	
	std::cout << "Forward: ";
	taiga::forward_traverse<char>(tree, print_value);
	std::cout << '\n';

	std::cout << "Symmetric: ";
	taiga::symmetric_traverse<char>(tree, print_value);
	std::cout << '\n';

	std::cout << "Backward: ";
	taiga::backward_traverse<char>(tree, print_value);
	std::cout << '\n';

	std::cout << "Level: ";
	taiga::level_traverse<char>(tree, print_value);
	std::cout << '\n';
	
	auto print_with_indent = [] (auto const& v, int level)
	{
		while (level--) std::cout << '\t';
		std::cout << v << '\n';
	};
	
	std::cout << "Forward with indent:\n";
	forward_traverse_with_indent<char>(tree, print_with_indent);
	std::cout << '\n';

	auto thread_tree = taiga::make_thread_tree<char>({'5', '3', '4', '7', '9', '2', '0', '8', '6', '1'});

	std::cout << "Forward with thread: ";
	taiga::forward_traverse<char>(thread_tree, print_value);
	std::cout << '\n';

	std::cout << "Symmetric forward with thread: ";
	taiga::symmetric_forward_traverse<char>(thread_tree, print_value);
	std::cout << '\n';

	std::cout << "Symmetric backward with thread: ";
	taiga::symmetric_backward_traverse<char>(thread_tree, print_value);
	std::cout << '\n';

	return 0;
}
