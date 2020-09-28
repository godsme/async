//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_WEAK_PTR_H
#define ASYNC_WEAK_PTR_H

#include "shared_ptr.h"

template<typename T>
struct weak_ptr {
   weak_ptr() = default;
   weak_ptr(shared_ptr<T> const& p)
      : ptr_{const_cast<shared_ptr_ctrl_block*>(p.ptr_.raw())} { add_ref(); }

   template<typename R, typename = std::enable_if_t<std::is_base_of_v<T, R>>>
   weak_ptr(weak_ptr<R> const& rhs) noexcept : ptr_{rhs.ptr_} { add_ref(); }

   weak_ptr(weak_ptr const& rhs) noexcept : ptr_{rhs.ptr_} { add_ref(); }

   template<typename R, typename = std::enable_if_t<(std::is_base_of_v<T, R> || std::is_same_v<T, R>)>>
   weak_ptr(weak_ptr<R>&& rhs) noexcept : ptr_{rhs.ptr_}{
      rhs.ptr_ = nullptr;
   }

   auto operator=(weak_ptr const& rhs) noexcept -> weak_ptr& {
      release();
      ptr_ = rhs.ptr_;
      add_ref();
      return *this;
   }

   auto operator=(weak_ptr&& rhs) noexcept -> weak_ptr& {
      rhs.ptr_ = std::exchange(ptr_, rhs.ptr_);
      return *this;
   }

   ~weak_ptr() noexcept { release(); }

   auto lock() noexcept -> shared_ptr<T> {
      if(ptr_ == nullptr) return {};
      return {intrusive_ptr_upgrade_weak(ptr_)};
   }

   auto expired() const noexcept -> bool {
      return use_count() == 0;
   }

   auto use_count() const noexcept -> std::size_t {
      return ptr_ == nullptr ? 0 : ptr_->use_count();
   }

   auto release() noexcept -> void {
      if(ptr_ != nullptr) {
         intrusive_ptr_release_weak(ptr_);
         ptr_ = nullptr;
      }
   }

private:
   auto add_ref() noexcept -> void {
      if(ptr_ != nullptr) intrusive_ptr_add_weak_ref(ptr_);
   }
private:
   shared_ptr_ctrl_block* ptr_{};
};

#endif //ASYNC_WEAK_PTR_H
