#include <exception>
#include <list>
#include <optional>
#include <random>
#include <vector>
#include <utility>

namespace opened
{
template<typename K, typename V>
using Cell = std::optional<std::pair<K, V>>;
	
template<typename K, typename V>
using Table = std::vector<Cell<K, V>>;

template<typename K>
struct LinearProbing
{
	K const k{0};
	size_t const n{0};

	size_t operator()(size_t i)
	{
		return (k + i) % n;
	}
};

template<typename K>
struct QuadraticProbing
{
	K const k{0};
	size_t const n{0};

	size_t operator()(size_t i)
	{
		return (k + i * i) % n;
	}
};

size_t pseudo_value(size_t n, unsigned init)
{
	std::default_random_engine engine{init};
	return std::uniform_int_distribution<size_t>{1, n - 1}(engine);
}

template<typename K>
struct RandomProbing
{
	K const k{0};
	size_t const n{0};
	size_t const p{pseudo_value(n, k % n)};
	
	size_t operator()(size_t i)
	{
		return (k + i * p) % n;
	}
};

template<typename K>
struct DoubleProbing
{
	K const k{0};
	size_t const n{0};
	size_t const p{pseudo_value(n, k)};
	
	size_t operator()(size_t i)
	{
		return (k + i * p) % n;
	}
};

template<typename K, typename V, typename F = LinearProbing<K>>
class Hash
{
public:
	using type_key = K;
	
	explicit Hash(size_t n) : data_(n) {}
	Table<K, V> const& data() const { return data_; } 
	
	void insert(K k, V const& v)
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p])
			{
				data_[p] = {k, v};
				return;
			}
			if (data_[p]->first == k)
			{
				throw std::runtime_error{"already placed"};
			}
		}
		throw std::runtime_error{"no place"};
	}

	V const& at(K k) const
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p])
			{
				throw std::runtime_error{"no key"};
			}
			if (data_[p]->first == k)
			{
				return data_[p]->second;
			}
		}
		throw std::runtime_error{"no key"};
	}

	V extract(K k)
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p])
			{
				throw std::runtime_error{"no key"};
			}
			if (data_[p]->first == k)
			{
				auto const v = data_[p]->second;
				//data_[i].reset(); удаление требует перехэшироване
				return v;
			}
		}
		throw std::runtime_error{"no key"};
	}
	
private:
	Table<K, V> data_;
};


template<typename K, typename V, typename F = LinearProbing<K>>
class SortedHash
{
public:
	using type_key = K;
	
	explicit SortedHash(size_t n) : data_(n) {}
	Table<K, V> const& data() const { return data_; } 
	
	void insert(K k, V const& v)
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p])
			{
				data_[p] = {k, v};
				return;
			}
			if (data_[p]->first > k)
			{
				auto item = data_[p];
				data_[p] = {k, v};
				insert(item->first, item->second);
				return;
			}
			if (data_[p]->first == k)
			{
				throw std::runtime_error{"already placed"};
			}
		}
		throw std::runtime_error{"no place"};
	}

	V const& at(K k) const
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p] || data_[p]->first > k)
			{
				throw std::runtime_error{"no key"};
			}
			if (data_[p]->first == k)
			{
				return data_[p]->second;
			}
		}
		throw std::runtime_error{"no key"};
	}

	V extract(K k)
	{
		auto const n = data_.size();
		F probing{k, n};
		for (auto i = 0; i < n; ++i)
		{
			auto const p = probing(i);
			if (!data_[p] || data_[p]->first > k)
			{
				throw std::runtime_error{"no key"};
			}
			if (data_[p]->first == k)
			{
				auto const v = data_[p]->second;
				//data_[i].reset(); удаление требует перехэшироване
				return v;
			}
		}
		throw std::runtime_error{"no key"};
	}
	
private:
	Table<K, V> data_;
};

template<typename K, typename V>
std::ostream& operator<<(std::ostream& out, typename std::pair<K, V> const& c)
{
	return out << '(' << c.first << ',' << c.second << ')';
}

template<typename K, typename V, typename F = LinearProbing<K>>
std::ostream& operator<<(std::ostream& out, Hash<K, V, F> const& h)
{
	auto const& table = h.data();
	if (table[0])
	{
		out << 0 << ':' << *table[0] << ' ';
	}
	for (auto i = 1; i < table.size(); ++i)
	{
		if (table[i] && !table[i-1])
		{
			out << '\n';
		}
		if (table[i])
		{
			out << i << ':' << *table[i] << ' ';
		}
	}
	return out << '\n';
}


template<typename K, typename V, typename F = LinearProbing<K>>
std::ostream& operator<<(std::ostream& out, SortedHash<K, V, F> const& h)
{
	auto const& table = h.data();
	if (table[0])
	{
		out << 0 << ':' << *table[0] << ' ';
	}
	for (auto i = 1; i < table.size(); ++i)
	{
		if (table[i] && !table[i-1])
		{
			out << '\n';
		}
		if (table[i])
		{
			out << i << ':' << *table[i] << ' ';
		}
	}
	return out << '\n';
}

} // opened
