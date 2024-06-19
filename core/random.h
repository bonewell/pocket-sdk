#ifndef _RANDOM_H_
#define _RANDOM_H_

#include <random>
#include <vector>

namespace core
{

template<typename T>
void random_elements(std::vector<T> & v)
{
	auto const t = static_cast<unsigned int>(time(nullptr));
	std::default_random_engine engine{t};

	auto const maxi = static_cast<int>(v.size()) - 1;
	for (auto i = 0; i < maxi - 1; ++i)
	{
		auto const j = std::uniform_int_distribution<int>{i, maxi}(engine);
		std::swap(v[i], v[j]);
	}
}

std::vector<long long> exclusive_random(
	long long min, long long max, int count)
{
	auto const t = static_cast<unsigned int>(time(nullptr));
	std::default_random_engine engine{t};

	auto const maxi = max - min;
	std::vector<long long> range(count);
	for (auto i = 0; i < range.size(); ++i)
	{
		auto const j = std::uniform_int_distribution<long long>{i % maxi, maxi}(engine);
		range[i] = j + min;
	}
	return range;
}		

std::vector<long long> exclusive_random(
	long long min, long long max)
{
	return exclusive_random(min, max, max - min + 1);
}

} // namespace core

#endif // _RANDOM_H_
