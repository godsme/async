
//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_ABSTRACT_FUTURE_H
#define ASYNC_ABSTRACT_FUTURE_H

struct abstract_future {
   virtual auto on_registered() noexcept -> void = 0;
   virtual ~abstract_future() noexcept = default;
};

#endif //ASYNC_ABSTRACT_FUTURE_H
