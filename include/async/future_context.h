//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_ACTOR_FUTURE_CONTEXT_H
#define ASYNC_ACTOR_FUTURE_CONTEXT_H

#include <async/abstract_future.h>
#include <unordered_set>
#include <memory>
#include <algorithm>

struct future_registry {
   using future_handle = std::shared_ptr<abstract_future>;

   auto register_future(future_handle future) noexcept -> void {
      future->on_registered();
      futures_.emplace(std::move(future));
   }

   auto unregister_future(abstract_future* future) noexcept -> void {
      for(auto&& obj : futures_) {
         if(obj.get() == future) {
            futures_.erase(obj);
            return;
         }
      }
   }

   auto empty() const noexcept -> bool {
      return futures_.empty();
   }

private:
   std::unordered_set<future_handle> futures_;
};

struct future_context : future_registry {

};

#endif //ASYNC_ACTOR_FUTURE_CONTEXT_H
