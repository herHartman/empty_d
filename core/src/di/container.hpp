#pragma once

#include <array>
#include <type_traits>
#include <variant>

namespace di_container {

template <typename T> struct JustType {
  using type = T;
};

template <typename... Ts> struct Dependecies {};

template <typename Dependency, typename... Args> struct DependencyFactory {

  JustType<Dependency> dependency;
  Dependecies<Args...> deps;

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
    constexpr std::array<std::variant<Deps...>, sizeof...(Deps)> deps;
    
    for (constexpr auto dep : deps) {
      if constexpr (std::holds_alternative<T>(dep)) {
          
      } 
    }
  }


};

}; // namespace di_container
