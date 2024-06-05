#pragma once

#include <cstddef>
#include <cstring>
#include <iterator>
#include <memory>
#include <optional>
#include <string>
#include <string_view>

std::size_t common_long_prefix(std::string_view x, std::string_view key) {
  for (int i = 0; i < key.size(); ++i) {
    if (i == x.size() || x[i] == key[i]) {
      return i;
    }
  }
  return key.size();
}

namespace http::web {

template <typename Value, typename KCharT> struct rd_tree_node {
  typedef Value value_type;

  typedef std::basic_string<KCharT, std::char_traits<KCharT>,
                            std::allocator<KCharT>>
      key_type;
  typedef rd_tree_node<value_type, key_type> *base_ptr;
  typedef const rd_tree_node<value_type, key_type> *const_base_ptr;

  base_ptr next;
  base_ptr minor;
  base_ptr parent;
  std::optional<value_type> value_field;

  static inline std::optional<value_type> traverse_for_value(base_ptr *x) {
    while (x && !x->value) {
      x = x->minor;
    }
    return x ? x->value : std::nullopt;
  }

  static base_ptr rd_tree_incremenent(base_ptr x) noexcept {}

  static const_base_ptr rd_tree_incremenent(const_base_ptr x) noexcept {}

  static base_ptr rd_tree_incremenent_local(base_ptr x) noexcept {
    base_ptr traverse_node = x->minor ? x->minor : x;
    if (traverse_node == x) {
      while (traverse_node && !traverse_node->next) {
        traverse_node = x->parent;
      }
      if (traverse_node) {
        traverse_node = traverse_node->next;
      }
    }
    return traverse_for_value(x);
  }
};

template <typename Value, typename KCharT,
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
    typedef Value *reference;
    typedef Value *pointer;

    typedef std::forward_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    iterator() = default;
    explicit iterator(base_ptr x) : node(x) {}
    reference operator*() const { return node->value_field; }
    pointer operator->() const { return std::__addressof(node->value_field); }

    iterator &operator++() {
      node = node_type::rd_tree_incremenent(node);
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

  radix_tree_map() = default;
  radix_tree_map(const radix_tree_map &x) = default;
  radix_tree_map(const radix_tree_map &&x) = default;
  radix_tree_map &operator=(const radix_tree_map &) = default;
  radix_tree_map &operator=(radix_tree_map &&) = default;

  iterator begin() noexcept {
    return root_.minor ? ++iterator(root_.minor) : iterator(root_);
  }

  iterator end() noexcept { return iterator(root_); }

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

  std::pair<iterator, bool> insert_internal(const value_type &x) {
    const key_type key = x.first;
    mapped_type &value = x.second;

    if (!root_.minor) {
      auto *new_node = node_type(key, value);
      root_.minor = new_node;
      return {iterator(root_.minor, true)};
    }

    node_type *traverse_node = root_.minor;

    while (traverse_node) {
      std::size_t common_prefix_len =
          common_long_prefix(traverse_node->key, key);

      if (!common_prefix_len) {
        if (!traverse_node->next) {
          traverse_node->next = new node_type(key, value);
          return {iterator(traverse_node->next), true};
        }
        traverse_node = traverse_node->next;
      } else if (common_prefix_len == traverse_node->key.size() &&
                 common_prefix_len == key.size()) {
        break;
      } else if (common_prefix_len < traverse_node->key.size()) {
        auto *new_node =
            new node_type(traverse_node->key.substr(common_prefix_len,
                                                    traverse_node->key.size()),
                          traverse_node->value_field);
        new_node->parent = traverse_node;
        traverse_node->minor = new_node;
        traverse_node->key.erase(0, common_prefix_len);
        if (common_prefix_len == key.size()) {
          new_node->key = key;
          return {iterator(new_node, true)};
        }
        key = key.substr(common_prefix_len, key.size());
        traverse_node = new_node;
      }
    }
    return {iterator(&root_, false)};
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
