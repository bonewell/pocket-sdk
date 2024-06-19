#ifndef _POWER_H_
#define _POWER_H_

#include <vector>

#include <cmath>

namespace core
{

double power(double v, int p)
{
	bool negative = p < 0;
	p = std::abs(p);

	std::vector<double> a{1, double(v)};
	for (int n = 1; n + 1 <= p; n *= 2)
	{
		auto const& last = a.back();
		a.push_back(last*last);
	}

	auto res = a[0];
	for (size_t i = 1; p != 0; ++i, p >>= 1)
	{
		if ((p & 1) == 1)
		{
			res *= a[i];
		}
	}
	return negative ? 1/res : res;
}

//! [deprecated]
long long pwr_mod(long long n,
				  long long p,
				  long long m)
{
	long long c = 1;
	while (p--)
	{
		c = (c * n) % m;
	}
	return c;
}

long long power_mod(long long n,
				 	long long p,
					long long m)
{
	bool negative = p < 0;
	p = std::abs(p);

	std::vector<long long> a{1, n % m};
	for (long long n = 1; n + 1 <= p; n *= 2)
	{
		auto const& last = a.back();
		a.push_back((last*last) % m);
	}

	auto res = a[0];
	for (size_t i = 1; p != 0; ++i, p >>= 1)
	{
		if ((p & 1) == 1)
		{
			res = (res * a[i]) % m;
		}
	}
	return negative ? 1/res : res;
}

} // namespace core

#endif // _POWER_H_
