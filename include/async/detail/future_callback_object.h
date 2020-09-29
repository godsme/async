//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_CALLBACK_OBJECT_H
#define ASYNC_FUTURE_CALLBACK_OBJECT_H

#include <async/detail/future_callback_base.h>
#include <async/detail/future_object.h>
#include <async/failure_handler.h>

namespace detail {
   template<typename R, typename F, typename A, typename = void>
   struct future_callback_object;

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F, typename A>
   struct future_callback_object<R, F, A, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>, A const &>>>
      : future_callback_base<R, F, A> {
      using super = future_callback_base<R, F, A>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         if constexpr (std::is_void_v<R>) {
            if constexpr (std::is_void_v<A>) {
               super::f_();
               future_object<void>::set_value();
            } else {
               super::f_(super::subject_->get_value());
               super::set_value();
            }
         } else {
            if constexpr (std::is_void_v<A>) {
               future_object<R>::set_value(super::f_());
            } else {
               super::set_value(super::f_(super::subject_->get_value()));
            }
         }
         super::do_commit();
      }
   };
}

#endif //ASYNC_FUTURE_CALLBACK_OBJECT_H
