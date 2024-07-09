#pragma once

#include <array>
#include <optional>
#include <type_traits>
#include <variant>
#include <iostream>

namespace di_container {

template <typename T> struct JustType {
  using type = T;
};

template <typename... Ts> struct Dependecies {};

template <typename Dependency, typename... Args> struct DependencyFactory {

  constexpr DependencyFactory() = default;

  static constexpr JustType<Dependency> dependency{};
  static constexpr Dependecies<Args...> deps{};

  Dependency createDependency() {
    return Dependency(DependencyCreator<Args>().createDependency()...);
  }

  template <typename T> struct DependencyCreator {
    T createDependency() { return T(); }
  };

  template <typename T, typename... Deps>
  struct DependencyCreator<DependencyFactory<T, Deps...>> {
    T createDependency() {
      return T(DependencyCreator<Deps>().createDependency()...);
    }
  };

};

template <typename T, typename... Ts>
constexpr DependencyFactory<T, Ts...> pushFront(DependencyFactory<Ts...>) {
  return {};
}

template <typename... Ts, typename T>
constexpr DependencyFactory<T, Ts...> pushFront(DependencyFactory<Ts...>,
                                                JustType<T>) {
  return {};
}

template <typename T, typename... Ts>
constexpr DependencyFactory<Ts...> popFront(DependencyFactory<T, Ts...>) {
  return {};
}

template <typename T, typename... Ts>
constexpr bool contains(DependencyFactory<Ts...>) {
  return (... || std::is_same_v<T, Ts>);
}



template <typename... Deps> struct Container {
  
  template <typename T> Container<Deps..., T> registerProvider() {
    return {};
  }

    template <typename T> Container<Deps..., T> registerProvider(T) {
    return {};
  }


  template<typename... T> Container<Deps..., T...> registerProviders() {
    return {};
  }
  
  template<typename... T> Container<Deps..., T...> registerProviders(T... factories) {
    return {};
  }

  template <typename T> constexpr decltype(auto) resolve() {
    using deps_t = std::variant<Deps...>;
    constexpr std::array<deps_t, sizeof...(Deps)> deps {Deps()...};
    std::optional<T> dependency = std::nullopt;
    
    auto dependencyGetter = [&dependency](auto&& arg) -> void {
      using factoryType = std::decay_t<decltype(arg)>;
      using targetType =  std::decay_t<decltype(factoryType::dependency)>::type;

      if constexpr (std::is_same_v<T, targetType>) {
        dependency.emplace(arg.createDependency());
      }
      std::cout << typeid(targetType).name() << std::endl;
    };

    for (auto dep : deps) {
        std::visit(dependencyGetter, dep);
        if (dependency) return dependency;
    }
    return dependency;
    // static_assert(false, "Dependency is not exist");
  }
};

}; // namespace di_container
