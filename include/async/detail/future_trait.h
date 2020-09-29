//
// Created by Darwin Yuan on 2020/9/29.
//

#ifndef ASYNC_FUTURE_TRAIT_H
#define ASYNC_FUTURE_TRAIT_H

template<typename T>
struct future;

namespace detail {
   template<typename T>
   struct future_trait;

   template<typename T>
   struct future_trait<future<T>> {
      using type = T;
   };

   template<typename T>
   using future_trait_t = typename future_trait<T>::type;
}

#endif //ASYNC_FUTURE_TRAIT_H
