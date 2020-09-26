//
// Created by Darwin Yuan on 2020/9/26.
//

#ifndef ASYNC_STATUS_T_H
#define ASYNC_STATUS_T_H

#include <cstdint>

enum status_t : uint64_t {
   ok,
   failed,
   invalid_data,
   out_of_memory
};

#endif //ASYNC_STATUS_T_H
