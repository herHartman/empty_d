#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <optional>
#include "http/web/radix_tree_map.h"

using simple_tree = http::web::radix_tree_map<int, char>;
using simple_comparator = http::web::prefix_comparator<simple_tree::key_type>;

TEST(RadixTreeTest, RadixTreeInsert) {
  simple_tree tree;
  
  tree.insert({"test",4});
  tree.insert({"abc",2});
  tree.insert({"tes", 5});
  tree.insert({"bag", 6});
  tree.insert({"bag/123", 11});
  tree.insert({"bag/123/test", 12});
  tree.insert({"garden", 7});
  tree.insert({"gar", 8});
  tree.insert({"ba", 127});

  std::cout << "after insert" << std::endl;

  std::optional<int> r1 = tree.lookup("bag/123");

  std::cout << r1.value_or(-1) << std::endl;

  auto iter = tree.begin();

  for (auto iter : tree) {
    std::cout << "next iter " << iter << std::endl;
  }
}

TEST(RadixTreeTest, ComparatorWithDynamicParams) {
  simple_comparator prefix_comparator;
  simple_tree::key_type k1 = "/test/{:id}/test";
  simple_tree::key_type k2 = "/test/{:simple_id}/border";
  simple_comparator::result_type result1 = prefix_comparator.find_common_prefix(k1, k2);

  EXPECT_EQ(result1.first, 12);
  EXPECT_EQ(result1.second, 19);

  simple_tree::key_type k3 = "/border/{:id}";
  simple_tree::key_type k4 = "/border/{:test}";

  simple_comparator::result_type result2 = prefix_comparator.find_common_prefix(k3, k4);

  EXPECT_EQ(result2.first, k3.size());  
  EXPECT_EQ(result2.second, k4.size());


  simple_tree::key_type k5 = "border/{:id}";
  simple_tree::key_type k6 = "/border/{:test}";

  simple_comparator::result_type result3 = prefix_comparator.find_common_prefix(k5, k6);
  EXPECT_EQ(result3.first, 0);
  EXPECT_EQ(result3.second, 0);
}

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}