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
   struct future_proxy_object<R, F, A> : future_callback_base<future_trait_t<R>, F, A>  {
      using subject_type = shared_ptr <future_object<A>>;
      using super = future_callback_base<future_trait_t<R>, F, A>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         if(!future_) {
            R future;
            if constexpr (std::is_void_v<A>) {
               future = super::f_();
            } else {
               future = super::f_(super::subject_->get_value());
            }

            if(future.object_) {
               future_ = future.object_;
               super::subject_.release();
               future_->add_observer(this);
            }
         } else {
            if constexpr (std::is_void_v<future_trait_t<R>>) {
               super::set_value();
            } else {
               super::set_value(future_->get_value());
            }
            future_.release();
            super::commit();
         }
      }

      auto cancel(status_t cause) noexcept -> void override {
         if(!future_) {
            super::cancel(cause);
         } else {
            future_->cancel(cause);
         }
      }

      typename R::object_type future_;
   };
}

#endif //ASYNC_FUTURE_PROXY_OBJECT_H
