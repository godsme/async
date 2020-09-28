//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_SHARED_PTR_H
#define ASYNC_SHARED_PTR_H

#include "shared_ptr_obj_storage.h"
#include <type_traits>

template<typename T>
struct weak_ptr;

template<typename T>
struct shared_ptr : private intrusive_ptr<shared_ptr_ctrl_block> {
   using super = intrusive_ptr<shared_ptr_ctrl_block>;

   shared_ptr() noexcept = default;
   shared_ptr(intrusive_ptr<shared_ptr_ctrl_block> ptr) noexcept
   : super(std::move(ptr)) {}

   template<typename R, typename = std::enable_if_t<std::is_base_of_v<T, R>>>
   shared_ptr(shared_ptr<R> const& rhs) noexcept : super{rhs}
   {}

   shared_ptr(shared_ptr const& rhs) noexcept = default;

   template<typename R, typename = std::enable_if_t<(std::is_base_of_v<T, R> || std::is_same_v<T, R>)>>
   shared_ptr(shared_ptr<R>&& rhs) noexcept : super{std::move(rhs)}
   {}

   auto operator=(shared_ptr const&) noexcept -> shared_ptr& = default;
   auto operator=(shared_ptr&&) noexcept -> shared_ptr& = default;
   ~shared_ptr() = default;

   auto use_count() const noexcept -> std::size_t  {
      auto ptr = super::raw();
      return ptr == nullptr ? 0 : ptr->use_count();
   }

   auto unique() const noexcept -> bool {
      return use_count() == 1;
   }

   auto get() const noexcept -> const T* {
      return reinterpret_cast<T*>(super::raw()->get());
   }

   auto get() noexcept -> T* {
      return reinterpret_cast<T*>(super::raw()->get());
   }

   auto operator->()  const noexcept -> T const* { return get(); }
   auto operator*()   const noexcept -> T const& { return *get(); }

   auto operator->()  noexcept -> T* { return get(); }
   auto operator*()   noexcept -> T& { return *get(); }

   using super::operator!;
   using super::operator bool;
   using super::release;

   inline friend auto operator==(const shared_ptr& x, std::nullptr_t) noexcept -> bool {
      return !x;
   }

   inline friend auto operator==(std::nullptr_t, const shared_ptr& x) noexcept -> bool {
      return !x;
   }

   inline friend auto operator!=(const shared_ptr& x, std::nullptr_t) noexcept -> bool {
      return static_cast<bool>(x);
   }

   inline friend auto operator!=(std::nullptr_t, const shared_ptr& x) noexcept -> bool {
      return static_cast<bool>(x);
   }

   inline friend auto operator==(const shared_ptr& x, const T* y) noexcept -> bool {
      return x.ptr_ == y;
   }

   inline friend auto operator==(const T* x, const shared_ptr& y) noexcept -> bool {
      return x == y.ptr_;
   }

   inline friend auto operator!=(const shared_ptr& x, const T* y) noexcept -> bool {
      return x.ptr_ != y;
   }

   inline friend auto operator!=(const T* x, const shared_ptr& y) noexcept -> bool {
      return x != y.ptr_;
   }

   inline friend auto operator==(const shared_ptr& x, const shared_ptr& y) noexcept -> bool {
      return x.ptr_ == y.ptr_;
   }

   inline friend auto operator!=(const shared_ptr& x, const shared_ptr& y) noexcept -> bool {
      return x.ptr_ != y.ptr_;
   }

private:
   template<typename>
   friend struct weak_ptr;
};

template<typename T, typename ... Args>
auto make_shared(page_allocator& allocator, Args&& ... args) -> shared_ptr<T> {
   auto p = new (allocator) shared_ptr_obj_storage<T>{allocator, std::forward<Args>(args)...};
   if(p == nullptr) return {};

   return {intrusive_ptr<shared_ptr_ctrl_block>{&p->control_, false}};
}

#endif //ASYNC_SHARED_PTR_H
