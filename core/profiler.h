#ifndef _PROFILER_H_
#define _PROFILER_H_

#include <ctime>

namespace core
{

class Profiler
{
public:
	explicit Profiler()
	{
		started_ = std::clock();
	}
	
	double time_ms() const
	{
		auto finished = std::clock();
		return 1000.0 * (finished - started_) / CLOCKS_PER_SEC;
	}
	
private:
	std::clock_t started_;
};

} // namespace core

#endif // _PROFILER_H_
