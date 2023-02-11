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
