//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_OBJECT_BASE_H
#define ASYNC_FUTURE_OBJECT_BASE_H

#include <async/detail/future_observer.h>
#include <async/abstract_future.h>
#include <functional>
#include <deque>
#include <memory>
#include <async/future_context.h>

namespace detail {
   template<typename T>
   struct future_callback_trait {
      using type = std::function<auto(T) -> void>;
   };

   template<>
   struct future_callback_trait<void> {
      using type = std::function<auto() -> void>;
   };

   template<typename T>
   struct future_object_base : abstract_future {
      using callback_type = typename future_callback_trait<T>::type;
      using observer_type = std::weak_ptr<future_observer<T>>;

      explicit future_object_base(future_context& context) : context_{context} {}

      auto add_observer(observer_type observer) {
         observers_.emplace_back(std::move(observer));
      }

      auto on_promise_done() noexcept -> void {
         if (ready_ || !present_) return;
         ready_ = true;
         notify_observers();
      }

      auto ready() const noexcept -> bool { return ready_; }

      auto launch() {}

      virtual ~future_object_base() = default;

   protected:
      auto notify_observers() {
         for (auto &observer: observers_) {
            notify_observer(observer);
         }
         observers_.clear();
      }

   private:
      auto notify_observer(observer_type &observer) noexcept -> void {
         auto o = observer.lock();
         if (o) notify_observer(*o);
      }

      virtual auto notify_observer(future_observer<T> &) noexcept -> void = 0;

   private:
      future_context& context_;
      std::deque<observer_type> observers_;

   protected:
      bool present_{false};
      bool ready_{false};
   };
}

#endif //ASYNC_FUTURE_OBJECT_BASE_H
