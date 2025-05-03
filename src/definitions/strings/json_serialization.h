#pragma once

#include <boost/describe.hpp>
#include <boost/json.hpp>
#include <boost/mp11.hpp>
#include <boost/version.hpp>
#include <type_traits>

namespace empty_d::json_serialization {

template <class T,
          class D1 = boost::describe::describe_members<
              T, boost::describe::mod_public | boost::describe::mod_protected>,
          class D2 = boost::describe::describe_members<
              T, boost::describe::mod_private>,
          class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value &&
                                      !std::is_union<T>::value>>

void tag_invoke(boost::json::value_from_tag const &, boost::json::value &v,
                T const &t) {
  auto &obj = v.emplace_object();

  boost::mp11::mp_for_each<D1>(
      [&](auto D) { obj[D.name] = boost::json::value_from(t.*D.pointer); });
}

template <class T>
void extract(boost::json::object const &obj, char const *name, T &value) {
  value = boost::json::value_to<T>(obj.at(name));
}

template <class T,
          class D1 = boost::describe::describe_members<
              T, boost::describe::mod_public | boost::describe::mod_protected>,
          class D2 = boost::describe::describe_members<
              T, boost::describe::mod_private>,
          class En = std::enable_if_t<boost::mp11::mp_empty<D2>::value &&
                                      !std::is_union<T>::value>>

T tag_invoke(boost::json::value_to_tag<T> const &,
             boost::json::value const &v) {
  auto const &obj = v.as_object();

  T t{};

  boost::mp11::mp_for_each<D1>([&](auto D) {
    extract(obj, D.name, t.*D.pointer);
  });

  return t;
}

template <typename T, typename = void>
struct has_tag_invoke_value_from : std::false_type {};

template <typename T>
struct has_tag_invoke_value_from<
    T, std::void_t<decltype(tag_invoke(
           std::declval<const boost::json::value_from_tag &>(),
           std::declval<boost::json::value &>(), std::declval<const T &>()))>>
    : std::true_type {};

template <typename T>
constexpr bool has_tag_invoke_value_from_v =
    has_tag_invoke_value_from<T>::value;

template <typename T, typename = void>
struct has_tag_invoke_value_to : std::false_type {};

template <typename T>
struct has_tag_invoke_value_to<
    T, std::void_t<decltype(tag_invoke(
           std::declval<const boost::json::value_to_tag<T> &>(),
           std::declval<const boost::json::value &>()))>> : std::true_type {};

template <typename T>
constexpr bool has_tag_invoke_value_to_v = has_tag_invoke_value_to<T>::value;

} // namespace empty_d::json_serialization