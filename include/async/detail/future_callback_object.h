//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_CALLBACK_OBJECT_H
#define ASYNC_FUTURE_CALLBACK_OBJECT_H

#include <async/detail/future_observer.h>
#include <async/detail/future_object.h>
#include <async/failure_handler.h>

namespace detail {
   template<typename R, typename F, typename A, typename = void>
   struct future_callback_object;

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F, typename A>
   struct future_callback_object_base : future_object<R>, future_observer<A> {
      using subject_type = std::shared_ptr<future_object<A>>;
      future_callback_object_base(future_context& context, subject_type subject, F &&f)
         : future_object<R>(context), subject_{std::move(subject)}, f_{std::forward<F>(f)} {}

      using super = future_object<R>;

      auto on_future_fail(status_t cause) noexcept -> void override {
         super::on_fail(cause);
         commit();
      }

   protected:
      auto commit() noexcept -> void {
         future_object<R>::commit();
         subject_.reset();
      }
   protected:
      std::decay_t<F> f_;
      subject_type subject_;
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F, typename A>
   struct future_callback_object<R, F, A, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>, A const &>>>
      : future_callback_object_base<R, F, A> {
      using super = future_callback_object_base<R, F, A>;
      using super::super;

      auto on_future_ready(A const &value) noexcept -> void override {
         future_object<R>::set_value(super::f_(value));
         super::commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename F, typename A>
   struct future_callback_object<void, F, A, std::enable_if_t<std::is_invocable_r_v<void, std::decay_t<F>, A const &>>>
      : future_callback_object_base<void, F, A> {
      using super = future_callback_object_base<void, F, A>;
      using super::super;

      auto on_future_ready(A const &value) noexcept -> void override {
         super::f_(value);
         future_object<void>::set_value();
         super::commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename R, typename F>
   struct future_callback_object<R, F, void, std::enable_if_t<std::is_invocable_r_v<R, std::decay_t<F>>>>
      : future_callback_object_base<R, F, void> {
      using super = future_callback_object_base<R, F, void>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         future_object<R>::set_value(super::f_());
         super::commit();
      }
   };

   /////////////////////////////////////////////////////////////////////////////////////////////////
   template<typename F>
   struct future_callback_object<void, F, void, std::enable_if_t<std::is_invocable_r_v<void, std::decay_t<F>>>>
      : future_callback_object_base<void, F, void> {
      using super = future_callback_object_base<void, F, void>;
      using super::super;

      auto on_future_ready() noexcept -> void override {
         super::f_();
         future_object<void>::set_value();
         super::commit();
      }
   };
}

#endif //ASYNC_FUTURE_CALLBACK_OBJECT_H
