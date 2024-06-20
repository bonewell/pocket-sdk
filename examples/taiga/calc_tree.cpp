#include <iostream>

#include "taiga/tree.h"

enum class Operation
{
	Literal,
	Plus,
	Minus,
	Times,
	Divide,
	Negative
};

struct Literal {};

template<typename T>
struct Expression
{
	Operation op;
	T literal{0};
};

using ExpInt = Expression<int>;
using ExpDbl = Expression<double>;

template<typename T>
using Functor = std::function<T(T, T)>;

template<typename T>
Functor<T> get_functor(Operation op)
{
	switch (op)
	{
		case Operation::Plus: return std::plus<T>{};
		case Operation::Minus: return std::minus<T>{};
		case Operation::Times: return std::multiplies<T>{};
		case Operation::Divide: return std::divides<T>{};
		default: break;
	}
	return [](T, T) -> T { return 0; }; 
}

template<typename T>
auto calculate(taiga::Link<T> const& link) -> decltype(T::literal)
{
	if (!link) return 0;
	if (link->value.op == Operation::Literal) return link->value.literal;
	auto op = get_functor<decltype(T::literal)>(link->value.op);
	auto left = calculate(link->left);
	auto right = calculate(link->right);
	return op(left, right);
}

int main()
{
	auto exp1 = taiga::make_node<ExpInt>({Operation::Plus});
	exp1->left = taiga::make_node<ExpInt>({Operation::Divide});
	exp1->left->left = taiga::make_node<ExpInt>({Operation::Literal, 15});
	exp1->left->right = taiga::make_node<ExpInt>({Operation::Literal, 3});
	exp1->right = taiga::make_node<ExpInt>({Operation::Divide});
	exp1->right->left = taiga::make_node<ExpInt>({Operation::Literal, 24});
	exp1->right->right = taiga::make_node<ExpInt>({Operation::Literal, 6});
	
	auto res1 = calculate(exp1);
	std::cout << "(15 / 3) + (24 / 6) = " << res1 << '\n';

	auto exp2 = taiga::make_node<ExpInt>({Operation::Minus});
	exp2->left = taiga::make_node<ExpInt>({Operation::Times});
	exp2->left->left = taiga::make_node<ExpInt>({Operation::Literal, 8});
	exp2->left->right = taiga::make_node<ExpInt>({Operation::Literal, 12});
	exp2->right = taiga::make_node<ExpInt>({Operation::Times});
	exp2->right->left = taiga::make_node<ExpInt>({Operation::Literal, 14});
	exp2->right->right = taiga::make_node<ExpInt>({Operation::Literal, 32});

	auto res2 = calculate(exp2);
	std::cout << "8 x 12 - 14 * 32 = " << res2 << '\n';

	auto exp3 = taiga::make_node<ExpDbl>({Operation::Plus});
	exp3->left = taiga::make_node<ExpDbl>({Operation::Divide});
	exp3->left->left = taiga::make_node<ExpDbl>({Operation::Literal, 1});
	exp3->left->right = taiga::make_node<ExpDbl>({Operation::Literal, 2});
	exp3->right = taiga::make_node<ExpDbl>({Operation::Plus});
	exp3->right->left = taiga::make_node<ExpDbl>({Operation::Divide});
	exp3->right->left->left = taiga::make_node<ExpDbl>({Operation::Literal, 1});
	exp3->right->left->right = taiga::make_node<ExpDbl>({Operation::Literal, 4});
	exp3->right->right = taiga::make_node<ExpDbl>({Operation::Divide});
	exp3->right->right->left = taiga::make_node<ExpDbl>({Operation::Literal, 1});
	exp3->right->right->right = taiga::make_node<ExpDbl>({Operation::Literal, 20});
	auto res3 = calculate(exp3);
	std::cout << "1 / 2 + 1 / 4 + 1/ 20 = " << res3 << '\n';

	return 0;
}
