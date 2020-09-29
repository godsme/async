//
// Created by Darwin Yuan on 2020/9/29.
//

#ifndef ASYNC_TUPLE_TRAIT_T_H
#define ASYNC_TUPLE_TRAIT_T_H

#include <tuple>

namespace detail {
   template<typename ... Xs>
   struct type_list {};

   template<std::size_t I, typename SEQ, typename ... Xs>
   struct filter_out;

   template<std::size_t I, std::size_t ...Is, typename H, typename ... Xs>
   struct filter_out<I, std::index_sequence<Is...>, H, Xs...> {
      using type = typename filter_out<I+1, std::index_sequence<Is..., I>, Xs...>::type;
   };

   template<std::size_t I, std::size_t ...Is, typename ... Xs>
   struct filter_out<I, std::index_sequence<Is...>, void, Xs...> {
      using type = typename filter_out<I+1, std::index_sequence<Is...>, Xs...>::type;
   };

   template<std::size_t I, typename SEQ>
   struct filter_out<I, SEQ> {
      using type = SEQ;
   };

   template<typename T> struct wrapper_t {
      using type = T;
   };

   template <typename ... Xs>
   using tuple_index_t = typename filter_out<0, std::index_sequence<>, Xs...>::type;

   template<typename ... Xs, std::size_t ... Is>
   auto tuple_trait(std::tuple<Xs...> t, std::index_sequence<Is...>)
   -> std::tuple<std::decay_t<decltype(std::declval<typename std::decay_t<decltype(std::get<Is>(t))>::type>())> ...>;

   template<typename ... Xs>
   using tuple_trait_t = decltype(tuple_trait(std::tuple<wrapper_t<Xs>...>{}, tuple_index_t<Xs...>{}));
}

#endif //ASYNC_TUPLE_TRAIT_T_H
