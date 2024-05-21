#pragma once

#include <cstddef>
#include <cstring>
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
class radix_tree_map {
  struct node {
    std::string key;
    node *minor;
    node *next;
    std::optional<int> value;

    node() : key(std::string{}), minor(nullptr), next(nullptr), value(0) {}
    ~node() = default;

    node(std::string key, std::optional<int> value) 
      : minor(nullptr), next(nullptr), key(std::move(key)), value(value) {}

    [[nodiscard]] bool is_leaf() const {
      return minor == nullptr && next == nullptr;
    }

  };

  std::optional<int> lookup(const char* x, int n) {
    node* traverse_node = &root;
    std::size_t elements_found = 0;

    while (traverse_node) {
      std::size_t common_prefix_len = common_long_prefix(traverse_node->key, x);
      if (common_prefix_len == traverse_node->key.size()) {
        elements_found += common_prefix_len;
        if (elements_found == n) {
          return traverse_node->value;
        } else {
          traverse_node = traverse_node->next;
        }
      } else if (common_prefix_len < traverse_node->key.size()) {
        return std::nullopt;
      } else {
        traverse_node = traverse_node->next;
      }
    }
    return std::nullopt;
  }

  void insert(const std::string& key, int value) {
    node* traverse_node = &root;
    std::size_t elements_found = 0;

    while (traverse_node) {
      std::size_t common_prefix_len = common_long_prefix(traverse_node->key, key);
      if (!common_prefix_len) {
        traverse_node = traverse_node->minor;
      } else if (common_prefix_len < key.size() && common_prefix_len < traverse_node->key.size()) {
        std::string prefix = traverse_node->key.substr(common_prefix_len, traverse_node->key.size());
        traverse_node->key.erase(common_prefix_len);
        node* next = traverse_node->next;
        node* new_node = new node(prefix, std::nullopt);
      }
      
      traverse_node->minor = new node(key, value);
    }
  }
 
  node root;
};
} // namespace http::web
