cmake_minimum_required(VERSION 3.17)
project(async)

set(CMAKE_CXX_STANDARD 17)

include_directories(
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
        "${CMAKE_CURRENT_SOURCE_DIR}/third-party"
)

add_executable(async tests/catch.cpp  include/async/detail/future_object.h include/async/detail/future_observer.h include/async/detail/future_callback_object.h include/async/future.h include/async/detail/future_object_base.h include/async/future_context.h include/async/abstract_future.h include/async/detail/future_proxy_object.h include/async/promise.h tests/test_future.cpp)