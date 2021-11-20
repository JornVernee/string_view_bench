#include <string_view>
#include <cstring>

#include <benchmark/benchmark.h>

#include "lib.h"

// empty call as a baseline for call overhead
static void BM_empty(benchmark::State& state) {
  for (auto _ : state) {
    empty();
  }
}
BENCHMARK(BM_empty);

static void BM_by_view(benchmark::State& state) {
  const char* str = "Hello";
  size_t length = std::strlen(str);
  std::string_view sv(str, length);  
  for (auto _ : state) {
    benchmark::DoNotOptimize(sv); // force load
    by_view(sv);
  }
}
BENCHMARK(BM_by_view);

static void BM_by_view_construct_inline(benchmark::State& state) {
  for (auto _ : state) {
    by_view("Hello");
  }
}
BENCHMARK(BM_by_view_construct_inline);

static void BM_by_split(benchmark::State& state) {
  const char* str = "Hello";
  size_t length = std::strlen(str);
  for (auto _ : state) {
    benchmark::DoNotOptimize(str);
    benchmark::DoNotOptimize(length);
    by_split(str, length);
  }
}
BENCHMARK(BM_by_split);

static void BM_by_split_length_first(benchmark::State& state) {
  const char* str = "Hello";
  size_t length = std::strlen(str);
  for (auto _ : state) {
    benchmark::DoNotOptimize(str);
    benchmark::DoNotOptimize(length);
    by_split_length_first(length, str);
  }
}
BENCHMARK(BM_by_split_length_first);

BENCHMARK_MAIN();
