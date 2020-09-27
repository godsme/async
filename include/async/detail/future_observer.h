//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_OBSERVER_H
#define ASYNC_FUTURE_OBSERVER_H

#include <async/status_t.h>

namespace detail {
   struct future_observer {
      virtual auto on_future_ready() noexcept -> void = 0;
      virtual auto on_future_fail(status_t cause) noexcept -> void = 0;

      virtual ~future_observer() = default;
   };
}

#endif //ASYNC_FUTURE_OBSERVER_H
