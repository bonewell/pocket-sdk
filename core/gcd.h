#ifndef _GCD_H_
#define _GCD_H_

namespace core
{

long long gcd(long long a, long long b)
{
	while(b != 0)
	{
		long long r = a % b;
		a = b;
		b = r;
	}
	return a;
}

} // namespace core

#endif // _GCD_H_
