//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_PROMISE_H
#define ASYNC_PROMISE_H

#include <memory>
#include <async/detail/future_object.h>
#include <async/future.h>

template<typename T>
struct promise_base {
   auto valid() const noexcept -> bool {
      return !future_.expired();
   }

   auto get_future(future_context& context) -> future<T> {
      auto f = future_.lock();
      if(f == nullptr) {
         f = std::make_shared<detail::future_object<T>>(context);
         future_ = f;
      }

      return future<T>{context, f};
   }

   auto commit() noexcept -> void {
      auto f = future_.lock();
      if(f) {
         f->commit();
      }
   }

protected:
   std::weak_ptr<detail::future_object<T>> future_;
};

template<typename T>
struct promise : promise_base<T> {
   using super = promise_base<T>;

   template<typename V, typename = std::enable_if_t<std::is_convertible_v<std::decay_t<V>, T>>>
   auto set_value(V&& value) -> void {
      auto future = super::future_.lock();
      if(future) {
         future->set_value(std::forward<V>(value));
      }
   }

};

template<>
struct promise<void> : promise_base<void> {
   using super = promise_base<void>;

   auto set_value() -> void {
      auto future = super::future_.lock();
      if(future) {
         future->set_value();
      }
   }

   auto on_fail(status_t status) -> void {
      auto future = super::future_.lock();
      if(future) {
         future->on_fail(status);
      }
   }
};

template<typename T>
auto future<T>::sink(promise<T>& p) -> future<void> {
   return then([=](auto &&value) mutable -> void {
      p.set_value(std::forward<decltype(value)>(value));
   });
}

#endif //ASYNC_PROMISE_H
