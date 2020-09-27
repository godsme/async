//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_FUTURE_OBJECT_H
#define ASYNC_FUTURE_OBJECT_H

#include <async/detail/future_object_base.h>

namespace detail {
   template<typename T, typename = void>
   struct future_object;

   template<typename T>
   struct future_object<T, std::enable_if_t<std::is_move_constructible_v<T>>>
      : future_object_base<T> {
      using super = future_object_base<T>;
      using super::super;

      template<typename R, typename = std::enable_if_t<std::is_convertible_v<R, T>>>
      auto set_value(R &&value) noexcept -> void {
         if (super::present_) return;
         new(&storage_) T(std::forward<R>(value));
         super::present_ = true;
      }

      auto get_value() const noexcept -> T const & {
         return *reinterpret_cast<const T *>(&storage_);
      }

   private:
      std::aligned_storage_t<sizeof(T), alignof(T)> storage_;
   };

   template<>
   struct future_object<void> : future_object_base<void> {
      using super = future_object_base<void>;
      using super::super;

      auto set_value() noexcept -> void {
         present_ = true;
      }
   };
}

#endif //ASYNC_FUTURE_OBJECT_H
