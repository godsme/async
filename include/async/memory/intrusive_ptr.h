//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_INTRUSIVE_PTR_H
#define ASYNC_INTRUSIVE_PTR_H

#include <cstddef>
#include <algorithm>

template<typename T>
struct intrusive_ptr;

struct shared_ptr_ctrl_block;
auto intrusive_ptr_add_weak_ref(shared_ptr_ctrl_block* x) noexcept -> void;
auto intrusive_ptr_add_ref(shared_ptr_ctrl_block* x) noexcept -> void;
auto intrusive_ptr_release_weak(shared_ptr_ctrl_block* x) noexcept -> void;
auto intrusive_ptr_release(shared_ptr_ctrl_block* x) noexcept -> void;
auto intrusive_ptr_upgrade_weak(shared_ptr_ctrl_block* x) noexcept -> intrusive_ptr<shared_ptr_ctrl_block>;

template<typename T>
struct intrusive_ptr {
   intrusive_ptr() noexcept = default;
   intrusive_ptr(T* ptr, bool add_ref = true) noexcept
      : ptr_(ptr) {
      if(ptr && add_ref) intrusive_ptr_add_ref(ptr);
   }

   intrusive_ptr(const intrusive_ptr& another) noexcept
      : ptr_(another.ptr_) {
      if(ptr_) intrusive_ptr_add_ref(ptr_);
   }

   intrusive_ptr(intrusive_ptr&& another) noexcept
      : ptr_(another.ptr_) {
      another.ptr_ = nullptr;
   }

   ~intrusive_ptr() noexcept { release(); }

   auto operator=(intrusive_ptr const& other) noexcept -> intrusive_ptr& {
      release();
      ptr_ = other.ptr_;
      if(ptr_) intrusive_ptr_add_ref(ptr_);
      return *this;
   }

   auto operator=(intrusive_ptr&& other) noexcept -> intrusive_ptr& {
      other.ptr_ = std::exchange(ptr_, other.ptr_);
      return *this;
   }

   T* operator->()  const noexcept { return ptr_; }
   T& operator*()   const noexcept { return *ptr_; }

   auto operator!() const noexcept -> bool { return !ptr_; }

   auto release() -> void {
      if (ptr_) {
         intrusive_ptr_release(ptr_);
         ptr_ = nullptr;
      }
   }

   inline auto raw() const noexcept -> T const* { return ptr_; }
   inline auto raw() noexcept -> T* { return ptr_; }

   operator bool() const noexcept {
      return ptr_ != nullptr;
   }

   inline friend auto operator==(const intrusive_ptr& x, std::nullptr_t) noexcept -> bool {
      return !x;
   }

   inline friend auto operator==(std::nullptr_t, const intrusive_ptr& x) noexcept -> bool {
      return !x;
   }

   inline friend auto operator!=(const intrusive_ptr& x, std::nullptr_t) noexcept -> bool {
      return static_cast<bool>(x);
   }

   inline friend auto operator!=(std::nullptr_t, const intrusive_ptr& x) noexcept -> bool {
      return static_cast<bool>(x);
   }

   inline friend auto operator==(const intrusive_ptr& x, const T* y) noexcept -> bool {
      return x.ptr_ == y;
   }

   inline friend auto operator==(const T* x, const intrusive_ptr& y) noexcept -> bool {
      return x == y.ptr_;
   }

   inline friend auto operator!=(const intrusive_ptr& x, const T* y) noexcept -> bool {
      return x.ptr_ != y;
   }

   inline friend auto operator!=(const T* x, const intrusive_ptr& y) noexcept -> bool {
      return x != y.ptr_;
   }

   inline friend auto operator==(const intrusive_ptr& x, const intrusive_ptr& y) noexcept -> bool {
      return x.ptr_ == y.ptr_;
   }

   inline friend auto operator!=(const intrusive_ptr& x, const intrusive_ptr& y) noexcept -> bool {
      return x.ptr_ != y.ptr_;
   }

   inline friend auto operator<(intrusive_ptr const& x, intrusive_ptr const& y) noexcept -> bool {
      return x.ptr_ < y.ptr_;
   }

private:
   void swap(intrusive_ptr& other) noexcept {
      std::swap(ptr_, other.ptr_);
   }

private:
   T* ptr_{};
};

#endif //ASYNC_INTRUSIVE_PTR_H
