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
#include <async/status_t.h>
#include <async/failure_handler.h>

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

      auto set_fail_handler(failure_handler&& handler) noexcept -> void {
         if(f_on_fail_) return;
         if(!ready_) {
            f_on_fail_ = std::move(handler);
         } else if(!present_) {
            handler(failure_);
         }
      }

      auto move_observers(future_object_base<T>& to) noexcept -> void {
         for (auto &observer: observers_) {
            to.add_observer(observer);
         }
         observers_.clear();
      }

      auto add_observer(observer_type observer) {
         if(ready_) {
            notify_observer(observer);
         } else {
            observers_.emplace_back(std::move(observer));
         }
      }

      auto commit() noexcept -> void {
         if (ready_ ) return;
         ready_ = true;
         if(!present_ && f_on_fail_) f_on_fail_(failure_);
         notify_observers();
         destroy();
      }

      auto on_fail(status_t cause) noexcept -> void {
         failure_ = cause;
      }

      auto ready() const noexcept -> bool { return ready_; }

      auto launch() {}

      auto destroy() noexcept -> void {
         if(registered) {
            context_.unregister_future(this);
            registered = false;
         }
      }

      auto on_registered() noexcept -> void override {
         registered = true;
      }

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
         if (o) {
            if(present_) notify_observer(*o);
            else o->on_future_fail(failure_);
         }
      }

      virtual auto notify_observer(future_observer<T> &) noexcept -> void = 0;

   private:
      future_context& context_;
      std::deque<observer_type> observers_;
      status_t failure_{};
      failure_handler f_on_fail_;

   protected:
      bool registered{false};
      bool present_{false};
      bool ready_{false};
   };
}

#endif //ASYNC_FUTURE_OBJECT_BASE_H
