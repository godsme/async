//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_OBJECT_H
#define ASYNC_FUTURE_OBJECT_H

#include <async/future_observer.h>
#include <optional>
#include <type_traits>
#include <functional>
#include <deque>
#include <memory>
#include <variant>

template <typename T>
struct future_callback_trait {
   using type = std::function<auto (T) -> void>;
};

template <>
struct future_callback_trait<void> {
   using type = std::function<auto () -> void>;
};

template<typename T>
struct future_object_base {
   using callback_type = typename future_callback_trait<T>::type;
   using observer_type = std::weak_ptr<future_observer<T>>;

   auto set_callback(callback_type&&  callback) noexcept -> void {
      if(!callback_) callback_.emplace(std::move(callback));
   }

   auto has_callback() const noexcept -> bool {
      return callback_.has_value();
   }

   auto add_observer(observer_type observer) {
      observers_.emplace_back(std::move(observer));
   }

   auto on_promise_done() noexcept -> void {
      if (ready_ || !present_) return;
      ready_ = true;
      notify_callback();
      notify_observers();
   }

   auto ready() const noexcept -> bool { return ready_; }

   auto launch() {}

   virtual ~future_object_base() = default;

protected:
   auto notify_observers() {
      for(auto& observer: observers_) {
         notify_observer(observer);
      }
      observers_.clear();
   }

private:
   auto notify_observer(observer_type& observer) noexcept -> void {
      auto o = observer.lock();
      if(o) notify_observer(*o);
   }

   virtual auto notify_observer(future_observer<T>&) noexcept -> void = 0;
   virtual auto notify_callback() noexcept -> void = 0;

private:
   std::deque<observer_type> observers_;

protected:
   std::optional<callback_type> callback_;
   bool present_{false};
   bool ready_{false};
};

template<typename T, typename = void>
struct future_object;

template<typename T>
struct future_object<T, std::enable_if_t<std::is_move_constructible_v<T>> >
   : future_object_base<T> {
   using super = future_object_base<T>;

   template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, T>>>
   auto set_value(R&& value) noexcept -> void {
      if(super::present_) return;
      new (&storage_) T(std::forward<R>(value));
      super::present_ = true;
   }

   auto get_value() const noexcept -> T const& {
      return *reinterpret_cast<const T*>(&storage_);
   }

private:
   auto notify_observer(future_observer<T>& observer) noexcept -> void override {
      if(observer.pass_value()) observer.on_future_ready(get_value());
      else observer.on_future_ready();
   }

   auto notify_callback() noexcept -> void override {
      if(super::callback_) (*super::callback_)(get_value());
   }

private:
   std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
};

template<>
struct future_object<void> : future_object_base<void> {
   using super = future_object_base<void>;
   using future_void_callback_t = std::function<auto () -> void>;

   auto set_value() noexcept -> void {
      present_ = true;
   }

private:
   auto notify_observer(future_observer<void>& observer) noexcept -> void override {
      observer.on_future_ready();
   }

   auto notify_callback() noexcept -> void override {
      if(callback_) (*callback_)();
   }
};

#endif //ASYNC_FUTURE_OBJECT_H
