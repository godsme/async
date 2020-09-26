//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_H
#define ASYNC_FUTURE_H

#include <async/future_callback_object.h>

template<typename T>
struct future {
   using object_type = std::shared_ptr<future_object<T>>;

   future() noexcept = default;
   future(object_type object) noexcept
      : object_{std::move(object)}
   {}

   template<typename F, typename R = std::invoke_result_t<F, T>>
   auto map(F&& callback) noexcept -> future<R> {
      if(!object_) return {};

      if constexpr (std::is_same_v<void, R>) {
         if(!object_->has_callback()) {
            object_->set_callback(std::move(callback));
            return {object_};
         }
      }

      auto cb = std::make_shared<future_callback_object<R, F, T>>(object_, std::forward<F>(callback));
      if(cb != nullptr) object_->add_observer(cb);
      return {cb};
   }

   auto launch() {
      if(object_) object_->launch();
   }

private:
   object_type object_;
};

#endif //ASYNC_FUTURE_H
