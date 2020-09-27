//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_WHEN_ALL_H
#define ASYNC_WHEN_ALL_H

#include <async/future.h>

template<typename ... Ts>
auto when_all(future<Ts>& ...) {
}

#endif //ASYNC_WHEN_ALL_H
