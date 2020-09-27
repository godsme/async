//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_PROXY_OBJECT_H
#define ASYNC_FUTURE_PROXY_OBJECT_H

#include <async/detail/future_observer.h>
#include <async/detail/future_object.h>

template<typename T>
struct future;

namespace detail {
   template<typename R, typename F, typename A, typename = void>
   struct future_proxy_object;

   template<typename T>
   struct future_trait;

   template<typename T>
   struct future_trait<future<T>> {
      using type = T;
   };

   template<typename T>
   using future_trait_t = typename future_trait<T>::type;

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F, typename A>
   struct future_proxy_object_base : future_callback_base<future_trait_t<R>, F, A> {
      using subject_type = std::shared_ptr <future_object<A>>;
      using super = future_callback_base<future_trait_t<R>, F, A>;
      using super::super;

   protected:
      auto destroy() noexcept -> void {
         super::destroy();
         super::subject_.reset();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F, typename A>
   struct future_proxy_object<R, F, A, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>, A const &>>>
      : future_proxy_object_base<R, F, A> {
      using super = future_proxy_object_base<R, F, A>;
      using super::super;

      auto on_future_ready(A const &value) noexcept -> void override {
         auto future = super::f_(value);
         if(future.object_) {
            super::move_observers(*future.object_);
         }
         super::destroy();
      }
   };

   template<typename R, typename F>
   struct future_proxy_object<R, F, void, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>>>>
      : future_proxy_object_base<R, F, void> {
      using super = future_proxy_object_base<R, F, void>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         auto future = super::f_();
         if(future.object_) {
            super::move_observers(*future.object_);
         }
         super::destroy();
      }
   };
}

#endif //ASYNC_FUTURE_PROXY_OBJECT_H
