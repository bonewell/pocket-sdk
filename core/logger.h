#ifndef _LOGGER_H_
#define _LOGGER_H_

#include <ostream>
#include <iterator>

namespace core
{

class Logger
{
public:
	Logger(std::ostream& out): out_(out) {}

	template<typename C>
	void debug(C const& c)
	{
		std::copy(std::cbegin(c), std::cend(c),
			std::ostream_iterator<typename C::value_type>{out_, " "});
		out_ << '\n';
	}
	
private:
	std::ostream& out_;
};

} // namespace core

#endif // _LOGGER_H_
