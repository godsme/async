//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_OBSERVER_H
#define ASYNC_FUTURE_OBSERVER_H

namespace detail {
   template<typename T>
   struct future_observer {
      virtual auto pass_value() const noexcept -> bool { return true; }
      virtual auto on_future_ready(T const &) noexcept -> void = 0;
      virtual auto on_future_ready() noexcept -> void {}

      virtual ~future_observer() = default;
   };

   template<>
   struct future_observer<void> {
      virtual auto on_future_ready() noexcept -> void = 0;

      virtual ~future_observer() = default;
   };
}

#endif //ASYNC_FUTURE_OBSERVER_H
