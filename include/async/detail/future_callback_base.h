//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_FUTURE_CALLBACK_BASE_H
#define ASYNC_FUTURE_CALLBACK_BASE_H

#include <async/detail/future_observer.h>
#include <async/detail/future_object.h>
#include <async/failure_handler.h>

namespace detail {
   template<typename R, typename F, typename A>
   struct future_callback_base : future_object<R>, future_observer<A> {
      using subject_type = std::shared_ptr<future_object<A>>;
      future_callback_base
         ( future_context& context
         , subject_type subject
         , F&& f)
            : future_object<R>(context)
            , subject_{std::move(subject)}
            , f_{std::forward<F>(f)} {}

      using super = future_object <R>;

      auto cancel(status_t cause) noexcept -> void override {
         subject_->deregister_observer(this, cause);
         super::cancel(cause);
         subject_.reset();
      }

      auto on_future_fail(status_t cause) noexcept -> void override {
         super::on_fail(cause);
         commit();
      }

   protected:
      auto commit() noexcept -> void {
         super::commit();
         subject_.reset();
      }

   protected:
      std::decay_t<F> f_;
      subject_type subject_;
   };
}

#endif //ASYNC_FUTURE_CALLBACK_BASE_H
