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
      futures_.emplace(std::move(future));
   }

   auto unregister_future(future_handle& future) noexcept -> void {
      futures_.erase(future);
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
