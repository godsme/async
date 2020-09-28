//
// Created by Darwin Yuan on 2020/9/28.
//

#ifndef ASYNC_SHARED_PTR_OBJ_STORAGE_H
#define ASYNC_SHARED_PTR_OBJ_STORAGE_H

#include <utility>
#include <async/memory/page_allocator.h>
#include "shared_ptr_ctl_block.h"
#include "shared_ptr.h"

template<typename T>
struct shared_ptr_obj_storage  {
   template<typename ... Ts>
   shared_ptr_obj_storage(page_allocator& allocator, Ts&& ... args)
      : control_{allocator, data_dtor, block_dtor} {
      new (&value_) T{std::forward<Ts>(args)...};
   }

   auto operator new(std::size_t size, page_allocator& allocator) -> void * {
      return allocator.alloc(size, std::align_val_t{CACHE_LINE_SIZE});
   }

private:
   static auto data_dtor(void* ptr) -> void {
      auto p = reinterpret_cast<T*>(ptr);
      p->~T();
   }

   static auto block_dtor(shared_ptr_ctrl_block* ptr) noexcept -> void {
      auto& allocator = ptr->allocator();
      ptr->~shared_ptr_ctrl_block();
      allocator.free((void*)ptr, sizeof(shared_ptr_obj_storage<T>));
   }

public:
   shared_ptr_ctrl_block control_;

private:
   static_assert(sizeof(shared_ptr_ctrl_block) <= CACHE_LINE_SIZE);
   char padding[CACHE_LINE_SIZE - sizeof(control_)];
   union { T value_; };
};



#endif //ASYNC_SHARED_PTR_OBJ_STORAGE_H
