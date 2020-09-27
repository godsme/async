//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_WHEN_ALL_H
#define ASYNC_WHEN_ALL_H

#include <async/future.h>

template<typename ... Xs>
auto when_all(future_context& context, future<Xs>& ... futures) {
   return future<detail::tuple_trait_t<Xs...>>(context, futures...);
}

#endif //ASYNC_WHEN_ALL_H
