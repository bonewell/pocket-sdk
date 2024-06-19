#ifndef _FACTORS_H_
#define _FACTORS_H_

#include <vector>
#include <cmath>

namespace core
{

using Factors = std::vector<long long>;

Factors get_factors(long long n)
{
	Factors factors;
	
	while(n % 2 == 0)
	{
		factors.push_back(2);
		n /= 2;
	}

	auto max = std::sqrt(n);
	for (int i = 3; i <= max; i += 2)
	{
		while(n % i == 0)
		{
			factors.push_back(i);
			n /= i;
			max = std::sqrt(n);
		}
	}
	
	if (n > 0) factors.push_back(n);

	return factors;
}

} // namespace core

#endif // _FACTORS_H_
