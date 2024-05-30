#pragma once

#include <cstddef>
#include <cstring>
#include <iterator>
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

template<
  typename KCharT,
  typename Value,
  KCharT opening_character,
  KCharT closing_character
>
class radix_tree_map {
public:

  typedef std::basic_string<KCharT> key_type;
  typedef std::basic_string_view<KCharT> key_view_type;

  typedef Value value_type;
  typedef value_type& reference;
  typedef const value_type& const_reference;
  typedef value_type* pointer;
  typedef const pointer const_pointer;

    struct node {
      key_type key;
      node* next;
      node* minor;
      node* parent;
      std::optional<Value> value;

      node() = default;
      ~node() = default;

      node(std::string key, std::optional<int> value) 
        : next(nullptr), minor(nullptr), key(std::move(key)), value(value) {}

      [[nodiscard]] bool is_leaf() const {
        return next == nullptr && minor == nullptr;
      }
  };

    node root;

    void insert(std::string key, value_type value) {
    node* traverse_node = &root;
    std::size_t elements_found = 0;

    while (traverse_node) {
      std::size_t common_prefix_len = common_long_prefix(traverse_node->key, key);
      if (!common_prefix_len) {
        if (!traverse_node->next) {
          traverse_node->next = new node(key, value);
          break;
        } else {
          traverse_node = traverse_node->next;
        }
      } else if (common_prefix_len < key.size()) {
        if (common_prefix_len < traverse_node->key.size()) {
          node* new_node = new node(traverse_node->key.substr(common_prefix_len), traverse_node->value);
          new_node->minor = traverse_node->minor;
          traverse_node->minor = new_node;
          traverse_node->key = traverse_node->key.substr(common_prefix_len, traverse_node->key.size());
          traverse_node = new_node;
        }
        key = key.substr(common_prefix_len, key.size());
        if (!traverse_node->minor) {
          traverse_node->minor = new node(key, value);
          break;
        } else {
          traverse_node = traverse_node->minor;
        }
      }
    }
  }

  std::optional<value_type> lookup(std::string_view x) {
    node* traverse_node = &root;
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

  struct iterator {
    typedef Value value_type;
    typedef Value& reference;
    typedef Value* pointer;
    
    typedef std::bidirectional_iterator_tag iterator_category;
    typedef ptrdiff_t difference_type;

    typedef iterator self;

    iterator() noexcept = default;
    explicit iterator(node x) noexcept : node_(x) {}

    reference operator*() const noexcept;

    pointer operator->() const noexcept;

    self& operator++() noexcept {
      return *this;
    }

    const self operator++(int) noexcept;
    

    friend bool operator==(const self& x, const self& y) noexcept {
      return x.node_ == y.node_;
    }

    friend bool operator!=(const self& x, const self& y) noexcept {
      return x.node_ == y.node_;
    }

    node* node_;
  };

  std::optional<value_type> lookup(key_view_type key) {
    return std::nullopt;
  }

  radix_tree_map() = default;
  radix_tree_map(const radix_tree_map&) = default;
  radix_tree_map(radix_tree_map&&) = default;

private:
    inline std::optional<Value> traverse_for_value(node* x) {
      while (x && !x->value) {
        x = x->minor;
      }
      return x ? x->value : std::nullopt;
  } 

  node* tree_increment(node* x) {
    node* traverse_node = x->minor ? x->minor : x;
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

} // namespace http::web
