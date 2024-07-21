#include "di/container.hpp"
#include "gtest/gtest.h"
#include <gtest/gtest.h>
#include <optional>

using di_container::Container;
using di_container::DependencyFactory;

namespace {

class Dep1 {};

class Connection {};

class Dep2 {
public:
  Dep2() = delete;
  explicit Dep2(Connection connection) : connection(connection) {}
  Connection connection;
};

class Dep3 {};

class A {
public:
  A();

  A(Dep1 d1, Dep2 d2, Dep3 d3) : d1(d1), d2(d2), d3(d3) {}

  Dep1 d1;
  Dep2 d2;
  Dep3 d3;
};
} // namespace

template <typename T, typename... Args> T test_deduce_args(Args... args) {
  return T(args...);
}

TEST(DITest, DITestCreateDependency) {

  auto dep1Provider = DependencyFactory<Dep1>{};
  auto connProvider = DependencyFactory<Connection>{};
  auto dep2Provider = DependencyFactory<Dep2, decltype(connProvider)>{};
  auto dep3provider = DependencyFactory<Dep3>{};

  auto factory =
      DependencyFactory<A, decltype(dep1Provider), decltype(dep2Provider),
                        decltype(dep3provider)>{};
}