cmake_minimum_required(VERSION 3.18)

project(OrderCache VERSION 0.1.0)

include(CTest)

add_executable(
  OrderCacheTest
  main_test.cpp
  src/OrderCache.cpp
  src/SecurityId.cpp
  src/Order.cpp
  catch2/catch.cpp
)

target_include_directories(OrderCacheTest PRIVATE include)

target_link_libraries(
  OrderCacheTest
)

set_target_properties(OrderCacheTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

