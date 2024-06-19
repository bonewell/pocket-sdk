#ifndef _HEAP_H_
#define _HEAP_H_

#include <iostream>
#include <array>

namespace core
{

template<typename T, size_t N>
using Data = std::array<T, N>;

template<typename T, size_t N>
void fix_top(Data<T, N> & d, size_t count)
{
	size_t i = 0;
	while (true)
	{
		auto child1 = 2 * i + 1;
		auto child2 = 2 * i + 2;
		if (child1 >= count) child1 = i;
		if (child2 >= count) child2 = i;
		
		if ((d[i] >= d[child1]) && (d[i] >= d[child2])) break;
		
		auto swapi = d[child1] > d[child2] ? child1 : child2;
		std::swap(d[i], d[swapi]);
		i = swapi;
	}
}

template<typename T, size_t N>
void fix_heap(Data<T, N> & data, size_t index)
{
	while (index != 0)
	{
		auto parent = (index - 1) / 2;
		if (data[index] <= data[parent]) break;
		std::swap(data[index], data[parent]);
		index = parent;
	}
}

template<typename T, size_t N>
class Heap
{
public:
	Heap() = default;
	explicit Heap(Data<T, N> d) : data_{std::move(d)}, last_{d.size()} {}

	void push(T const& v) &
	{
		if (last_ == data_.size()) throw std::out_of_range{"Heap is full"};
		data_[last_] = v;
		fix_heap(data_, last_);
		++last_;
	}

	void push(T && v)
	{
		if (last_ == data_.size()) throw std::out_of_range{"Heap is full"};
		data_[last_] = v;
		fix_heap(data_, last_);
		++last_;
	}

	T pop()
	{
		if (last_ == 0) throw std::out_of_range{"Heap is empty"};
		auto v = data_[0];
		--last_;
		data_[0] = data_[last_];
		fix_top(data_, last_);
		return v;
	}
	
	T const& top()
	{
		return data_[0];
	}
	
	Data<T, N> const& data() const
	{
		return data_;
	}
	
	bool empty() const
	{
		return last_ == 0;
	}
	
	size_t size() const
	{
		return last_;
	}

private:
	Data<T, N> data_;
	size_t last_ = 0;
};

template<typename T, size_t N>
Heap<T, N> make_heap(Data<T, N> data)
{
	for (size_t i = 0; i < data.size(); ++i)
	{
		fix_heap(data, i);
	}
	
	return Heap{data};
}

template<typename T, size_t N>
std::ostream& operator<<(std::ostream& out, Heap<T, N> const& heap)
{
	auto d = heap.data();
	std::copy(
		std::begin(d),
		std::begin(d) + heap.size(),
		std::ostream_iterator<typename Data<T, N>::value_type>{out, " "});
	return out;
}

} // namespace core

#endif // _HEAP_H_
