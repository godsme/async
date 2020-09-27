//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_H
#define ASYNC_FUTURE_H

#include <async/detail/future_callback_object.h>
#include <async/detail/future_proxy_object.h>
#include <async/detail/when_all_object.h>
#include <async/status_t.h>

template<typename T>
struct future;

template<typename T>
struct promise;

template<typename T>
constexpr bool Is_Future = false;

template<typename T>
constexpr bool Is_Future<future<T>> = true;

template<typename T>
constexpr bool Is_Tuple = false;

template<typename ... Ts>
constexpr bool Is_Future<std::tuple<Ts...>> = true;

template<typename F, typename T>
struct invoke_result {
   constexpr static bool invokable = false;
};

template<typename F, typename ... Xs>
struct invoke_result<F, std::tuple<Xs...>> {
   using type = std::invoke_result_t<F, Xs...>;
   constexpr static bool invokable = true;
};

template<typename F, typename T>
using invoke_result_t = typename invoke_result<F, T>::type;

template<typename T>
struct future;

template<typename T>
struct future final {
   using object_type = std::shared_ptr<detail::future_object<T>>;

   future() noexcept = default;
   future(future_context& context, object_type object) noexcept
      : context_{&context}, object_{std::move(object)}
   {}

   template<typename ... Ys, typename = std::enable_if_t<(sizeof...(Ys) > 1)>>
   future(future_context& context, future<Ys>& ... futures) noexcept
      : context_{&context} {
      auto obj = std::make_shared<detail::when_all_object<Ys...>>(context, futures ...);
      if(obj != nullptr && obj->valid()) {
         obj->check_done();
         object_ = obj;
      }
   };

   template<typename F, typename R = std::invoke_result_t<F, T>, typename = std::enable_if_t<!Is_Future<R>>>
   auto then(F&& callback) noexcept -> future<R> {
      if(context_ == nullptr || !object_) return {};

      auto cb = std::make_shared<detail::future_callback_object<R, F, T>>(*context_, object_, std::forward<F>(callback));
      return {*context_, cb};
   }

   template<typename F, typename R = invoke_result_t<F, T>, typename = std::enable_if_t<!Is_Future<R>>>
   auto then(F&& f) noexcept -> future<invoke_result_t<F, T>> {
      if(context_ == nullptr || !object_) return {};

      auto nf = [=](auto && value) mutable -> R { return std::apply(f, value); };
      auto cb = std::make_shared<detail::future_callback_object<R, decltype(nf), T>>(*context_, object_,
                                                                                     std::move(nf));

      return future<R>{*context_, cb};
   }

   template<typename F, typename R = std::invoke_result_t<F, T>, typename = std::enable_if_t<Is_Future<R>>>
   auto then(F&& callback) noexcept -> R {
      if(context_ == nullptr || !object_) return {};

      auto cb = std::make_shared<detail::future_proxy_object<R, F, T>>(*context_, object_, std::forward<F>(callback));
      return R{*context_, cb};
   }

   template<typename F, typename = std::enable_if_t<std::is_invocable_r_v<void, F, status_t>>>
   auto fail(F&& on_fail) noexcept -> future<T>& {
      if(!object_) {
         on_fail(status_t::invalid_data);
      } else {
         object_->set_fail_handler(std::forward<F>(on_fail));
      }
      return *this;
   }

   auto cancel(status_t cause) noexcept -> void {
      if(object_) {
         object_->cancel(cause);
         object_.reset();
      }
   }

   auto sink(promise<T>& p) noexcept -> future<void>;

   inline auto valid() const noexcept -> bool {
      return static_cast<bool>(object_);
   }

   ~future() noexcept {
      if(object_ && object_.unique() && !object_->ready()) {
         context_->register_future(object_);
      }
   }

private:
   template<typename R, typename F, typename A, typename>
   friend struct detail::future_proxy_object;

   template<typename ... Xs>
   friend struct detail::when_all_object;

private:
   future_context* context_{};
   object_type object_;
};


#endif //ASYNC_FUTURE_H
