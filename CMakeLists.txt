cmake_minimum_required(VERSION 3.17)
project(async)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party"
)

add_executable(async tests/catch.cpp  include/async/detail/future_object.h include/async/detail/future_observer.h include/async/detail/future_callback_object.h include/async/future.h include/async/detail/future_object_base.h include/async/future_context.h include/async/abstract_future.h include/async/detail/future_proxy_object.h include/async/promise.h tests/test_future.cpp include/async/status_t.h include/async/failure_handler.h include/async/detail/future_callback_base.h include/async/memory/page_allocator.h include/async/memory/shared_ptr_ctl_block.h include/async/memory/shared_ptr_obj_storage.h include/async/memory/intrusive_ptr.h src/shared_ptr_ctrl_block.cpp include/async/memory/shared_ptr.h include/async/memory/weak_ptr.h tests/test_shared_ptr.cpp include/async/memory/make_shared.h include/async/detail/future_trait.h include/async/detail/tuple_trait_t.h)