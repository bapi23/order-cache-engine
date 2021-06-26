cmake_minimum_required(VERSION 3.18)

project(OrderCache VERSION 0.1.0)

include(CTest)
find_package(GTest REQUIRED)
include(GoogleTest)

add_executable(
  OrderCacheTest
  main_test.cpp
  src/OrderCache.cpp
)

target_include_directories(OrderCacheTest PRIVATE include)

target_link_libraries(
  OrderCacheTest
  GTest::GTest
  GTest::Main 
  pthread
)

include(GoogleTest)
gtest_discover_tests(OrderCacheTest)

set_target_properties(OrderCacheTest PROPERTIES
            CXX_STANDARD 17
            CXX_EXTENSIONS OFF
            )

