//
// Created by Darwin Yuan on 2020/9/27.
//

#ifndef ASYNC_FUTURE_CALLBACK_BASE_H
#define ASYNC_FUTURE_CALLBACK_BASE_H

#include <async/detail/future_observer.h>
#include <async/detail/future_object.h>
#include <async/failure_handler.h>
#include <async/memory/shared_ptr.h>

namespace detail {
   template<typename R, typename F, typename A>
   struct future_callback_base : future_object<R>, future_observer {
      using subject_type = shared_ptr<future_object<A>>;
      future_callback_base
         ( future_context& context
         , subject_type subject
         , F&& f)
            : future_object<R>(context)
            , subject_{std::move(subject)}
            , f_{std::forward<F>(f)} {
         if(subject_) subject_->add_observer(this);
      }

      using super = future_object <R>;

      auto cancel(status_t cause) noexcept -> void override {
         super::cancel(cause);
         detach_subject(cause);
      }

      auto on_future_fail(status_t cause) noexcept -> void override {
         super::on_fail(cause);
         do_commit();
      }

      ~future_callback_base() {
         detach_subject(status_t::ok);
      }

   protected:
      auto do_commit() noexcept -> void {
         super::commit();
         detach_subject(status_t::ok);
      }

      auto detach_subject(status_t cause) noexcept -> void {
         if(subject_) {
            subject_->deregister_observer(this, cause);
            subject_.release();
         }
      }

   protected:
      std::decay_t<F> f_;
      subject_type subject_;
   };
}

#endif //ASYNC_FUTURE_CALLBACK_BASE_H
