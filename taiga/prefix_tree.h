#ifndef _PREFIX_TREE_H_
#define _PREFIX_TREE_H_

#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace taiga
{

template<typename T> struct PrefixNode;

template<typename T>
using PrefixLink = std::unique_ptr<PrefixNode<T>>;

template<typename T>
using PrefixTree = PrefixLink<T>;

template<typename T>
struct PrefixNode
{
	using value_type = std::optional<T>;

	PrefixNode(std::string_view k, value_type&& v)
		: key{k} ,
		  value(std::forward<value_type>(v)) {}

	std::string key;
	value_type value;

	std::unordered_map<char, PrefixLink<T>> children;
};

template<typename T>
PrefixLink<T> make_prefix_node(
	std::string_view const& key = "",
	std::optional<T>&& value = std::nullopt)
{
	return std::make_unique<PrefixNode<T>>(key, std::forward<std::optional<T>>(value));
}

template<typename T>
void add_value(PrefixLink<T>& link, std::string_view key, std::optional<T>&& value)
{	
	if (!key.empty() && key == link->key)
	{
		link->value = std::forward<std::optional<T>>(value);
		return;
	}

	if (key.empty() && link->key.empty())
	{
		link->value = std::forward<std::optional<T>>(value);
		return;
	}

	if (key.empty() && !link->key.empty())
	{
		link->children[link->key[0]] = make_prefix_node(
			{link->key.data() + 1},
			std::forward<std::optional<T>>(link->value));
		link->key = "";
		link->value = std::forward<std::optional<T>>(value);
		return;
	}

	if (link->children.empty())
	{
		if (!link->key.empty())
		{
			link->children[link->key[0]] = make_prefix_node(
				{link->key.data() + 1},
				std::forward<std::optional<T>>(link->value));
			link->key = "";
			link->value = std::nullopt;
		}
	}

	if (auto found = link->children.find(key[0]); found == std::end(link->children))
	{
		link->children[key[0]] = make_prefix_node(
			{key.data() + 1},
			std::forward<std::optional<T>>(value));
		return;
	}
	
	add_value(link->children[key[0]], {key.data() + 1}, std::forward<std::optional<T>>(value));
}

template<typename T>
PrefixTree<T> make_prefix_tree(std::vector<std::pair<std::string, T>> const& data = {})
{
	PrefixLink<T> tree = make_prefix_node<T>();
	for (auto [key, value] : data)
	{
		add_value(tree, key, std::forward<std::optional<T>>(value));
	}
	return tree;
}

template<typename T>
T find(PrefixLink<T> const& link, std::string const& key)
{
	if (!link) throw "not found";
	if (key == link->key) return *link->value;

	if (auto found = link->children.find(key[0]); found != std::end(link->children))
	{
		return find(found->second, {key.data() + 1});
	}
	throw "not found";
}

} // namespace taiga

#endif // _PREFIX_TREE_H_
