//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_SHARED_PTR_CTL_BLOCK_H
#define ASYNC_SHARED_PTR_CTL_BLOCK_H

#include <atomic>
#include <cstddef>
#include <async/memory/intrusive_ptr.h>
#include <async/memory/page_allocator.h>

struct page_allocator;

template<typename T>
struct shared_ptr_obj_storage;

struct shared_ptr_ctrl_block {
   using data_destructor = void (*)(void*);
   using block_destructor = void (*)(shared_ptr_ctrl_block*);

   shared_ptr_ctrl_block
      ( page_allocator& allocator,
        data_destructor data_dtor,
        block_destructor block_dtor)
   : strong_refs_{1}
   , weak_refs_{1}
   , data_dtor_{data_dtor}
   , block_dtor_{block_dtor}
   , allocator_(allocator)
   {}

   auto get() noexcept -> void* {
      return reinterpret_cast<char*>(this) + CACHE_LINE_SIZE;
   }

   auto get() const noexcept -> void const* {
      return reinterpret_cast<const char*>(this) + CACHE_LINE_SIZE;
   }

   auto use_count() const noexcept -> std::size_t {
      return strong_refs_.load(std::memory_order_relaxed);
   }

public:
   inline friend auto intrusive_ptr_add_weak_ref(shared_ptr_ctrl_block* x) noexcept -> void {
      x->weak_refs_.fetch_add(1, std::memory_order_relaxed);
   }

   inline friend auto intrusive_ptr_add_ref(shared_ptr_ctrl_block* x) noexcept -> void {
      x->strong_refs_.fetch_add(1, std::memory_order_relaxed);
   }

   friend auto intrusive_ptr_release_weak(shared_ptr_ctrl_block* x) noexcept -> void;
   friend auto intrusive_ptr_release(shared_ptr_ctrl_block* x) noexcept -> void;

   friend auto intrusive_ptr_upgrade_weak(shared_ptr_ctrl_block* x) noexcept -> intrusive_ptr<shared_ptr_ctrl_block>;

private:
   template<typename T>
   friend struct shared_ptr_obj_storage;

   auto allocator() noexcept -> page_allocator& {
      return allocator_;
   }

private:
   std::atomic<std::size_t> strong_refs_;
   std::atomic<std::size_t> weak_refs_;
   const data_destructor data_dtor_;
   const block_destructor block_dtor_;
   page_allocator& allocator_;
};


#endif //ASYNC_SHARED_PTR_CTL_BLOCK_H
