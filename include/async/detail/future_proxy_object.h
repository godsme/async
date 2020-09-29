//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_PROXY_OBJECT_H
#define ASYNC_FUTURE_PROXY_OBJECT_H

#include <async/detail/future_observer.h>
#include <async/detail/future_object.h>
#include <async/detail/future_trait.h>

namespace detail {

   template<typename R, typename F, typename A>
   struct future_proxy_object : future_callback_base<future_trait_t<R>, F, A>  {
      using subject_type = shared_ptr <future_object<A>>;
      using super = future_callback_base<future_trait_t<R>, F, A>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         if(!future_) {
            auto future = get_future();
            if(future.object_) {
               future_ = future.object_;
               super::subject_.release();
               future_->add_observer(this);
            }
         } else {
            on_future_ready_();
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

   private:
      auto get_future() noexcept {
         if constexpr (std::is_void_v<A>) {
            return super::f_();
         } else {
            return super::f_(super::subject_->get_value());
         }
      }

      auto on_future_ready_() noexcept -> void {
         if constexpr (std::is_void_v<future_trait_t<R>>) {
            super::set_value();
         } else {
            super::set_value(future_->get_value());
         }
      }

      typename R::object_type future_;
   };
}

#endif //ASYNC_FUTURE_PROXY_OBJECT_H
