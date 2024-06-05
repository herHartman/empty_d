#include <gtest/gtest.h>
#include "http/web/radix_tree_map.h"

TEST(RadixTreeTest, RadixTreeInsert) {
  http::web::radix_tree_map<int, char> tree;
  tree.insert({"test",1});
  tree.insert({"abc",1});

  auto begin = *tree.begin();
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}