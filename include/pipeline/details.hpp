#pragma once
#include <tuple>

namespace pipeline {

template <typename T1, typename T2> class pipe_pair;

template <typename Fn, typename... Fns> class fork_into;

namespace details {

// is_tuple constexpr check
template <typename> struct is_tuple : std::false_type {};
template <typename... T> struct is_tuple<std::tuple<T...>> : std::true_type {};

template <typename Test, template <typename...> class Ref>
struct is_specialization : std::false_type {};

template <template <typename...> class Ref, typename... Args>
struct is_specialization<Ref<Args...>, Ref> : std::true_type {};

template <class F, size_t... Is> constexpr auto index_apply_impl(F f, std::index_sequence<Is...>) {
  return f(std::integral_constant<size_t, Is>{}...);
}

template <size_t N, class F> constexpr auto index_apply(F f) {
  return index_apply_impl(f, std::make_index_sequence<N>{});
}

// Unpacks Tuple into a parameter pack
// Calls f(parameter_pack)
template <class Tuple, class F> constexpr auto apply(Tuple t, F f) {
  return index_apply<std::tuple_size<Tuple>{}>([&](auto... Is) { return f(std::get<Is>(t)...); });
}

template <size_t N, typename T> constexpr decltype(auto) make_repeated_tuple(T t) {
  if constexpr (N == 1) {
    return t;
  } else {
    return make_repeated_tuple<N - 1>(std::tuple_cat(std::make_tuple(std::get<0>(t)), t));
  }
}

template <typename T, typename F, int... Is>
void for_each(T &&t, F f, std::integer_sequence<int, Is...>) {
  auto l = {(f(std::get<Is>(t)), 0)...};
}

template <typename... Ts, typename F> void for_each_in_tuple(std::tuple<Ts...> const &t, F f) {
  for_each(t, f, std::make_integer_sequence<int, sizeof...(Ts)>());
}

} // namespace details

} // namespace pipeline