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
         super::set_value(super::f_(super::subject_->get_value()));
         super::do_commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename F, typename A>
   struct future_callback_object<void, F, A, std::enable_if_t<std::is_invocable_r_v<void, std::decay_t<F>, A const &>>>
      : future_callback_base<void, F, A> {
      using super = future_callback_base<void, F, A>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         super::f_(super::subject_->get_value());
         super::set_value();
         super::do_commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F>
   struct future_callback_object<R, F, void, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>>>>
      : future_callback_base<R, F, void> {
      using super = future_callback_base<R, F, void>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         future_object<R>::set_value(super::f_());
         super::do_commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename F>
   struct future_callback_object<void, F, void, std::enable_if_t<std::is_invocable_r_v<void, std::decay_t<F>>>>
      : future_callback_base<void, F, void> {
      using super = future_callback_base<void, F, void>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         super::f_();
         future_object<void>::set_value();
         super::do_commit();
      }
   };
}

#endif //ASYNC_FUTURE_CALLBACK_OBJECT_H
