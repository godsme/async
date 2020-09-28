//
// Created by Darwin Yuan on 2020/9/28.
//

#include <async/memory/shared_ptr_ctl_block.h>

//////////////////////////////////////////////////////////////////////////////
auto intrusive_ptr_release_weak(shared_ptr_ctrl_block* x) noexcept -> void {
   if (x->weak_refs_ == 1
       || x->weak_refs_.fetch_sub(1, std::memory_order_acq_rel) == 1)
      x->block_dtor_(x);
}

//////////////////////////////////////////////////////////////////////////////
auto intrusive_ptr_release(shared_ptr_ctrl_block* x) noexcept -> void {
   if (x->strong_refs_.fetch_sub(1, std::memory_order_acq_rel) == 1) {
      x->data_dtor_(x->get());
      intrusive_ptr_release_weak(x);
   }
}

//////////////////////////////////////////////////////////////////////////////
auto intrusive_ptr_upgrade_weak(shared_ptr_ctrl_block* x) noexcept -> intrusive_ptr<shared_ptr_ctrl_block> {
   auto count = x->strong_refs_.load();
   while (count != 0) {
      if (x->strong_refs_.compare_exchange_weak(count, count + 1,
                                                std::memory_order_relaxed)) {
         return {x, false};
      }
   }

   return nullptr;
}