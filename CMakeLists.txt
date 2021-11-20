cmake_minimum_required(VERSION 3.14)
project(BenchStringView)

add_library(Lib SHARED libLib.cpp)
set_property(TARGET Lib PROPERTY CXX_STANDARD 17)

add_executable(Bench Bench.cpp)
set_property(TARGET Bench PROPERTY CXX_STANDARD 17)

find_package(benchmark CONFIG REQUIRED)

target_link_libraries(Bench benchmark::benchmark)
target_link_libraries(Bench Lib)

install(TARGETS
  Lib
  Bench
)