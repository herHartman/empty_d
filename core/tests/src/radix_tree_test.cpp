#include <gtest/gtest.h>

using namespace http::web;

TEST(TmpAddTest, CheckValues) {
 radix_tree_map<int, char> map;
 map.insert({"test",1});
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}