# string_view_bench

Dependency: https://github.com/google/benchmark

---

Note: the target functions are put in a DLL to force them to be out-of-line. I couldn't get `__declspec(noinline)` to work on MSVC. Using a library guarantees maximum out-of-line-ness, meaning we get the full effects of the calling convetion/ABI.

My conclusion, mostly from the clang-cl numbers is: The fact that the Windows x64 ABI passes string_view by-hidden-pointer, instead of in 2 registers, is not really a problem. Just use clang(-cl), and it is the same speed as passing a pointer & length separately.

---

Built on Windows with MSVC:

```powershell
> cl.exe /?
Microsoft (R) C/C++ Optimizing Compiler Version 19.28.29334 for x64
...
> mkdir build_win
> cd build_win
> cmake .. '-DCMAKE_BUILD_TYPE=Release' '-DCMAKE_INSTALL_PREFIX=.local' '-DCMAKE_PREFIX_PATH=<path to google/benchmark cmake conf>' -T 'host=x64' -A x64
> cmake --build . --config Release --target install
> .\.local\bin\Bench.exe --benchmark_repetitions=20 --benchmark_report_aggregates_only=true
```

Numbers with MSVC (Windows x64 ABI):

```text
Run on (8 X 3493 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 64 KiB (x4)
  L2 Unified 512 KiB (x4)
  L3 Unified 8192 KiB (x2)
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
BM_empty_mean                            2.45 ns         2.45 ns           20
BM_empty_median                          2.43 ns         2.41 ns           20
BM_empty_stddev                         0.070 ns        0.082 ns           20
BM_empty_cv                              2.85 %          3.34 %            20
BM_by_view_mean                          3.71 ns         3.71 ns           20
BM_by_view_median                        3.70 ns         3.68 ns           20
BM_by_view_stddev                       0.025 ns        0.039 ns           20
BM_by_view_cv                            0.67 %          1.06 %            20
BM_by_view_construct_inline_mean         7.70 ns         7.70 ns           20
BM_by_view_construct_inline_median       7.70 ns         7.67 ns           20
BM_by_view_construct_inline_stddev      0.010 ns        0.085 ns           20
BM_by_view_construct_inline_cv           0.13 %          1.11 %            20
BM_by_split_mean                         5.30 ns         5.30 ns           20
BM_by_split_median                       5.30 ns         5.31 ns           20
BM_by_split_stddev                      0.011 ns        0.035 ns           20
BM_by_split_cv                           0.22 %          0.66 %            20
BM_by_split_length_first_mean            5.58 ns         5.57 ns           20
BM_by_split_length_first_median          5.58 ns         5.58 ns           20
BM_by_split_length_first_stddev         0.008 ns        0.031 ns           20
BM_by_split_length_first_cv              0.14 %          0.56 %            20
```

Note: MSVC is at an unfair advantage because [DoNotOptimize is more expensive](https://github.com/google/benchmark/blob/b5bb9f0675acb7d4093d482e89883febae75d103/include/benchmark/benchmark.h#L407). So, the by_view loop gets 1 extra out of line call, and the by_split* ones get 2 (one for each use of DoNotOptimize). I guess I should add a benchmark of DoNotOptimize as a baseline as well...

The fact that MSVC uses a redundant copy in the case of the BM_by_view_construct_inline case is... unfortunate.

---

Built on Windows with clang-cl (I didn't rebuild the benchmark lib with clang-cl):

```powershell
> clang-cl.exe --version
clang version 10.0.0
Target: x86_64-pc-windows-msvc
Thread model: posix
...
> mkdir build_clang
> cd build_clang
> cmake .. '-DCMAKE_BUILD_TYPE=Release' '-DCMAKE_INSTALL_PREFIX=.local' '-DCMAKE_PREFIX_PATH=<path to google/benchmark cmake conf>' -T ClangCL -A x64
> cmake --build . --config Release --target install
> .\.local\bin\Bench.exe --benchmark_repetitions=20 --benchmark_report_aggregates_only=true
```

Numbers with clang-cl (Windows x64 ABI):

```
Run on (8 X 3493 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 64 KiB (x4)
  L2 Unified 512 KiB (x4)
  L3 Unified 8192 KiB (x2)
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
BM_empty_mean                            1.86 ns         1.86 ns           20
BM_empty_median                          1.86 ns         1.86 ns           20
BM_empty_stddev                         0.009 ns        0.021 ns           20
BM_empty_cv                              0.50 %          1.15 %            20
BM_by_view_mean                          1.86 ns         1.86 ns           20
BM_by_view_median                        1.86 ns         1.84 ns           20
BM_by_view_stddev                       0.002 ns        0.021 ns           20
BM_by_view_cv                            0.13 %          1.15 %            20
BM_by_view_construct_inline_mean         1.87 ns         1.87 ns           20
BM_by_view_construct_inline_median       1.87 ns         1.88 ns           20
BM_by_view_construct_inline_stddev      0.003 ns        0.020 ns           20
BM_by_view_construct_inline_cv           0.18 %          1.10 %            20
BM_by_split_mean                         1.87 ns         1.87 ns           20
BM_by_split_median                       1.87 ns         1.88 ns           20
BM_by_split_stddev                      0.002 ns        0.020 ns           20
BM_by_split_cv                           0.12 %          1.05 %            20
BM_by_split_length_first_mean            2.14 ns         2.14 ns           20
BM_by_split_length_first_median          2.14 ns         2.13 ns           20
BM_by_split_length_first_stddev         0.003 ns        0.019 ns           20
BM_by_split_length_first_cv              0.15 %          0.87 %            20
```

"Have you tried LLVM?" (It's great)

---

Built on WSL/Linux with GCC:

```bash
$ gcc --version
gcc (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0
...
$ mkdir build_wsl
$ cd build_wsl
$ cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=.local -DCMAKE_PREFIX_PATH=<path to google/benchmark cmake conf>
$ cmake --build . --config Release --target install
$ LD_LIBRARY_PATH=.local/lib .local/bin/Bench --benchmark_repetitions=20 --benchmark_report_aggregates_only=true
```

Numbers with GCC (SysV x64 ABI):

```text
Run on (8 X 3500 MHz CPU s)
Load Average: 0.52, 0.58, 0.59
-----------------------------------------------------------------------------
Benchmark                                   Time             CPU   Iterations
-----------------------------------------------------------------------------
BM_empty_mean                            2.80 ns         2.80 ns           20
BM_empty_median                          2.80 ns         2.79 ns           20
BM_empty_stddev                         0.013 ns        0.036 ns           20
BM_empty_cv                              0.46 %          1.30 %            20
BM_by_view_mean                          2.90 ns         2.90 ns           20
BM_by_view_median                        2.90 ns         2.92 ns           20
BM_by_view_stddev                       0.010 ns        0.029 ns           20
BM_by_view_cv                            0.35 %          1.02 %            20
BM_by_view_construct_inline_mean         3.16 ns         3.16 ns           20
BM_by_view_construct_inline_median       3.16 ns         3.14 ns           20
BM_by_view_construct_inline_stddev      0.003 ns        0.034 ns           20
BM_by_view_construct_inline_cv           0.09 %          1.08 %            20
BM_by_split_mean                         2.92 ns         2.92 ns           20
BM_by_split_median                       2.92 ns         2.92 ns           20
BM_by_split_stddev                      0.003 ns        0.015 ns           20
BM_by_split_cv                           0.10 %          0.51 %            20
BM_by_split_length_first_mean            2.91 ns         2.91 ns           20
BM_by_split_length_first_median          2.91 ns         2.92 ns           20
BM_by_split_length_first_stddev         0.003 ns        0.020 ns           20
BM_by_split_length_first_cv              0.12 %          0.70 %            20
```
