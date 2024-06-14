#pragma once

#include <cstddef>
#include <cstring>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <tuple>

std::size_t common_long_prefix(std::string_view x, std::string_view key) {
  for (int i = 0; i < key.size(); ++i) {
    if (i == x.size() || x[i] != key[i]) {
      return i;
    }
  }
  return key.size();
}

namespace http::web {

template <typename Key> struct prefix_comparator {
  using key_iterator = Key::const_iterator;
  using key_type = Key;

  using result_type = std::pair<key_iterator, key_iterator>;

  result_type find_common_prefix(const key_type &key1, const key_type &key2) {
    key_iterator it1 = key1.cbegin();
    key_iterator it2 = key2.cbegin();

    key_iterator end1 = key1.cend();
    key_iterator end2 = key2.cend();

    if (*it1 != *it2) {
      return {end1, end2};
    }

    while (it1 != end1) {
      if (it2 == end2 || *it1 != *it2) {

        if (*it1 == '{') {
          it1 = std::find(it1, end1, '}');
        }

        if (*it2 == '{') {
          it2 = std::find(it2, end2, '}');
        }

        return {it1, it2};
      } else if (*it1 == '{') {
        it1 = std::find(it1, end1, '}');
        it2 = std::find(it2, end2, '}');

        if (it1 + 1 == end1 && it2 + 1 == end2)
          return {it1, it2};
      }
      ++it1;
      ++it2;
    }
    return {it1, it2};
  }
};

template <typename Value, typename KCharT> struct rd_tree_node {
  typedef Value value_type;

  typedef std::basic_string<KCharT, std::char_traits<KCharT>,
                            std::allocator<KCharT>>
      key_type;
  typedef rd_tree_node<value_type, KCharT> *base_ptr;
  typedef const rd_tree_node<value_type, KCharT> *const_base_ptr;

  base_ptr next = nullptr;
  base_ptr minor = nullptr;
  base_ptr parent = nullptr;
  std::optional<value_type> value_field;
  key_type key;

  rd_tree_node() = default;
  rd_tree_node(const rd_tree_node &) = default;
  rd_tree_node(rd_tree_node &&) = default;

  rd_tree_node(key_type key, std::optional<value_type> value)
      : key(std::move(key)), value_field(std::move(value)), next(nullptr),
        minor(nullptr), parent(nullptr) {}

  rd_tree_node(key_type key, std::optional<value_type> value, base_ptr minor,
               base_ptr parent, base_ptr next)
      : key(std::move(key)), value_field(std::move(value)), next(next),
        minor(minor), parent(parent) {}

  static inline base_ptr traverse_for_value(base_ptr x) {
    while (x && !x->value_field) {
      x = x->minor;
    }
    return x;
  }

  static base_ptr rd_tree_incremenent(base_ptr x) noexcept {}

  static const_base_ptr rd_tree_incremenent(const_base_ptr x) noexcept {}

  static base_ptr rd_tree_incremenent_local(base_ptr x) noexcept {
    base_ptr traverse_node = x->minor ? x->minor : x;
    if (traverse_node == x) {
      while (!traverse_node->key.empty() && !traverse_node->next) {
        traverse_node = traverse_node->parent;
      }
      if (!traverse_node->key.empty()) {
        traverse_node = traverse_node->next;
      } else {
        return traverse_node;
      }
    }
    return traverse_for_value(traverse_node);
  }
};

template <typename Value, typename KCharT,
          typename PrefixComparator =
              prefix_comparator<typename rd_tree_node<Value, KCharT>::key_type>,
          typename Allocator = std::allocator<std::pair<
              const typename rd_tree_node<Value, KCharT>::key_type, Value>>>
class radix_tree_map {
public:
  typedef rd_tree_node<Value, KCharT> node_type;
  typedef rd_tree_node<Value, KCharT> *ptr_node_type;

  typedef radix_tree_map<Value, KCharT> tree_type;

  class iterator {
  public:
    typedef node_type::base_ptr base_ptr;
    typedef node_type::key_type key_type;
    typedef Value mapped_type;
    typedef std::pair<const key_type, mapped_type> value_type;
    typedef Value &reference;
    typedef Value *pointer;

    typedef std::forward_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    iterator() = default;
    explicit iterator(base_ptr x) : node(x) {}
    reference operator*() const { return node->value_field.value(); }
    pointer operator->() const {
      return std::addressof(node->value_field.value());
    }

    iterator &operator++() {
      node = node_type::rd_tree_incremenent_local(node);
      return *this;
    }

    iterator &operator++(int) {
      iterator tmp = *this;
      node = node_type::rd_tree_incremenent(node);
      return *this;
    }

    bool operator==(const iterator &x) const { return node == x.node; }
    bool operator!=(const iterator &x) const { return node != x.node; }

    base_ptr node;
  };

  typedef node_type::key_type key_type;
  typedef Value mapped_type;
  typedef std::pair<const key_type, mapped_type> value_type;
  typedef value_type *pointer;
  typedef const value_type *const_pointer;
  typedef value_type *reference;
  typedef const value_type &const_reference;
  typedef size_t size_type;
  typedef ptrdiff_t difference_type;
  typedef Allocator allocator_type;

  typedef iterator iterator;
  typedef PrefixComparator comparator;

  // radix_tree_map() = default;
  // radix_tree_map(const radix_tree_map &x) = default;
  // radix_tree_map(const radix_tree_map &&x) = default;
  // radix_tree_map &operator=(const radix_tree_map &) = default;
  // radix_tree_map &operator=(radix_tree_map &&) = default;

  iterator begin() noexcept {
    return root_.minor ? iterator(root_.minor) : end();
  }

  iterator end() noexcept { return iterator(std::addressof(root_)); }

  value_type &operator[](const key_type &key) {}

  value_type &&operator[](key_type &&key) {}

  value_type &at(const key_type &key) {}

  const value_type &at(const key_type &key) const {}

  std::pair<iterator, bool> insert(const value_type &x) {
    return insert_internal(x);
  }

  std::pair<iterator, bool> insert(value_type &&x) {
    return insert_internal(std::move(x));
  }

private:
  node_type root_;
  comparator comparator_;

  std::pair<iterator, bool> insert_internal(const value_type &x) {
    key_type key = x.first;
    const mapped_type &value = x.second;

    if (!root_.minor) {
      auto *new_node = new node_type(key, value);
      root_.minor = new_node;
      new_node->parent = std::addressof(root_);
      return {iterator(root_.minor), true};
    }

    node_type *traverse_node = root_.minor;

    while (traverse_node) {
      typename comparator::result_type compare_prefix_result =
          comparator_.find_common_prefix(traverse_node->key, key);

      if (compare_prefix_result.first ==
              traverse_node->key.begin() + traverse_node->key.size() - 1 &&
          compare_prefix_result.second == key.begin() + key.size() - 1) {
        break;
      }

      if (compare_prefix_result.first == traverse_node->key.end() &&
          compare_prefix_result.second == key.end()) {
        if (!traverse_node->next) {
          auto *new_node = new node_type(key, value);
          traverse_node->next = new_node;
          new_node->parent = traverse_node->parent;
          return {iterator(new_node), true};
        }
        traverse_node = traverse_node->next;
      } else if (common_prefix_len < traverse_node->key.size()) {

        auto *new_node =
            new node_type(traverse_node->key.substr(common_prefix_len,
                                                    traverse_node->key.size()),
                          std::move(traverse_node->value_field));

        new_node->parent = traverse_node;
        new_node->minor = traverse_node->minor;
        traverse_node->minor = new_node;

        traverse_node->key.erase(common_prefix_len, traverse_node->key.size());

        if (common_prefix_len == key.size()) {
          traverse_node->value_field = value;
          return {iterator(new_node), true};
        }
        traverse_node->value_field = std::nullopt;
        key = key.substr(common_prefix_len, key.size());
        traverse_node = new_node;
      } else if (common_prefix_len == traverse_node->key.size()) {
        key = key.substr(common_prefix_len, key.size());
        if (!traverse_node->minor) {
          auto *new_node = new node_type(key, std::move(value));
          traverse_node->minor = new_node;
          new_node->parent = traverse_node;
          return {iterator(new_node), true};
        }
        traverse_node = traverse_node->minor;
      }
    }
    return {iterator(&root_), false};
  }

  std::optional<mapped_type> lookup(const key_type &x) {
    node_type *traverse_node = &root_;
    std::size_t elements_found = 0;

    while (traverse_node) {
      std::size_t common_prefix_len = common_long_prefix(traverse_node->key, x);
      if (common_prefix_len == traverse_node->key.size()) {
        elements_found += common_prefix_len;
        if (elements_found == x.size()) {
          return traverse_node->value;
        } else {
          traverse_node = traverse_node->minor;
        }
      } else if (common_prefix_len < traverse_node->key.size()) {
        return std::nullopt;
      } else {
        traverse_node = traverse_node->minor;
      }
    }
    return std::nullopt;
  }
};

} // namespace http::web
