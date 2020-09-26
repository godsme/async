//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FAILURE_HANDLER_H
#define ASYNC_FAILURE_HANDLER_H

#include <functional>
#include <async/status_t.h>

using failure_handler = std::function<auto (status_t) -> void>;

#endif //ASYNC_FAILURE_HANDLER_H
