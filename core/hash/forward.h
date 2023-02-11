#include <exception>
#include <list>
#include <vector>
#include <utility>

namespace forward
{
template<typename K, typename V>
class Bucket
{
public:
	using Type = std::pair<K, V>;
	using Data = std::list<Type>;
	
	Data const& data() const { return data_; }

	void emplace(K k, V v)
	{
		auto found = find(data_, k);
		if (found != std::end(data_) && found->first == k)
		{
			throw std::runtime_error{"already presented"};
		}
		data_.emplace(found, k, v);
	}
	
	V const& at(K k) const
	{	
		auto found = find(data_, k);
		if (found != std::end(data_) && found->first == k)
		{
			return found->second;
		}
		throw std::out_of_range{"not found"};
	}
	
	V extract(K k)
	{
		auto found = find(data_, k);
		if (found != std::end(data_) && found->first == k)
		{
			auto const value = found->second;
			data_.erase(found);
			return value;
		}
		throw std::out_of_range{"not found"};
	}

protected:
	using Iterator = typename std::list<Type>::const_iterator;

private:
	virtual Iterator find(Data const& data, K k) const
	{
		return std::find_if(
			std::begin(data),
			std::end(data),
			[k](auto const& p) { return p.first == k; });
	}
	
	Data data_;
};

template<typename K, typename V>
class SortedBucket : public Bucket<K, V>
{
private:
	typename Bucket<K, V>::Iterator
	find(typename Bucket<K, V>::Data const& data, K k) const override
	{
		return std::find_if(
			std::begin(data),
			std::end(data),
			[k](auto const& p) { return p.first >= k; });
	}
};

template<typename B>	
using Table = std::vector<B>;

template<typename K, typename V, typename B = Bucket<K, V>>
class Hash
{
public:
	using type_key = K;

	explicit Hash(size_t n) : data_(n) {}
	Table<B> const& data() const { return data_; }
	
	void insert(K k, V const& v)
	{
		auto const i = k % data_.size();
		data_[i].emplace(k, v);
	}

	V const& at(K k) const
	{
		auto const i = k % data_.size();
		return data_[i].at(k);
	}

	V extract(K k)
	{
		auto const i = k % data_.size();
		return data_[i].extract(k);
	}
	
private:
	Table<B> data_;
};

template<typename K, typename V>
std::ostream& operator<<(std::ostream& out, Bucket<K, V> const& b)
{
	for (auto const& [k, v]: b.data())
	{
		out << '(' << k << ", " << v << ") ";
	}
	return out;
}

template<typename K, typename V, typename B = Bucket<K, V>>
std::ostream& operator<<(std::ostream& out, Hash<K, V, B> const& h)
{
	auto const& table = h.data();
	for (auto i = 0; i < table.size(); ++i)
	{
		out << i << ": " << table[i] << '\n';
	}
	return out;
}

} // forward
