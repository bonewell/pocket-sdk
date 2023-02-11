#include <cmath>
#include <vector>

using EratosthenesTable = std::vector<bool>;

EratosthenesTable sift(long long number)
{
	EratosthenesTable table(number + 1, false);
	for(long long i = 4; i <= number; i += 2)
	{
		table[i] = true;
	}
	
	long long next = 3;
	long long const stop = std::sqrt(number);
	while(next <= stop)
	{
		for (long long i = next * next; i <= number; i += next)
		{
			table[i] = true;
		}
		next += 2;
		while(next <= number && table[next]) next += 2;
	}
	return table;
}
