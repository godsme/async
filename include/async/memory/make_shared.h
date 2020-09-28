//
// Created by Darwin Yuan on 2020/9/29.
//

#ifndef ASYNC_MAKE_SHARED_H
#define ASYNC_MAKE_SHARED_H

#include <async/future_context.h>
#include <async/memory/shared_ptr.h>

template<typename T, typename ... Args>
auto make_shared(future_context& context, Args&& ... args) -> shared_ptr<T> {
   auto p = new (context) shared_ptr_obj_storage<T>{context, context, std::forward<Args>(args)...};
   if(p == nullptr) return {};

   return {intrusive_ptr<shared_ptr_ctrl_block>{&p->control_, false}};
}

template<typename T, typename ... Args>
auto alloc_shared(page_allocator& allocator, Args&& ... args) -> shared_ptr<T> {
   auto p = new (allocator) shared_ptr_obj_storage<T>{allocator, std::forward<Args>(args)...};
   if(p == nullptr) return {};

   return {intrusive_ptr<shared_ptr_ctrl_block>{&p->control_, false}};
}

#endif //ASYNC_MAKE_SHARED_H
