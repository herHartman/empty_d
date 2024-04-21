//
// Created by chopk on 18.04.24.
//

#ifndef EMPTY_D_COMPILE_TIME_CONTAINERS_H
#define EMPTY_D_COMPILE_TIME_CONTAINERS_H

template<typename T, T... Xs>
struct list {
    using item_type = T;
    static constexpr auto size = sizeof ... (Xs);
};

template<typename U> using item_t = typename U::item_type;
template<typename U> constexpr auto size_v = U::size;
template<typename U> constexpr bool is_empty_v = (size_v<U> == 0);

template<typename U, item_t<U> X>
struct append;

template<typename T, T... Xs, T X>
struct append<list<T, Xs...>, X> { using type = list<T, Xs..., X>; };

template<typename U, typename V>
struct contact;

template<typename T, T... Xs, T... Ys>
struct contact<list<T, Xs...>, list<T, Ys...>> { using type = list<T, Xs..., Ys...>; };

#endif //EMPTY_D_COMPILE_TIME_CONTAINERS_H
