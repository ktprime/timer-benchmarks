# timerqueue-benchmark

分别使用最小堆、红黑树、时间轮实现定时器，再用随机数据对插入、删除、循环做性能测试。

关于定时器的简单介绍，可以参考[这篇文章](https://www.ibm.com/developerworks/cn/linux/l-cn-timers/index.html)


# 如何构建本项目

* 下载[CMake](https://cmake.org/download/)
* `mkdir build && cd build && cmake ..`



# 性能测试

## 算法复杂度

复杂度比较：

```
algo   | Add()    | Cancel() | Tick()   | implement
--------------------------------------------------------
最小堆 | O(log N) | O(N)     | O(1)     | src/PQTimer.h
红黑树 | O(log N) | O(N)     | O(log N) | src/TreeTimer.h
时间轮 | O(1)     | O(1)     | O(1)     | src/WheelTimer.h
时间轮 | O(1)     | O(1)     | O(1)     | src/WheelTimer2.h 改进实现
桶hash | O(1)     | O(1)     | O(n)     | src/BucketTimer.h
哈希   | O(1)     | O(1)     | O(n)     | src/HashTimer.h
```

最小堆和红黑树的Cancel均使用for遍历找到id做删除，所以都是O(N)复杂度。

```
AMD 1700 3.0GHz win10/Ubu18.04 GCC 9.3 -O3
============================================================================
PQTimerAdd                                                  49.31ms    20.28
TreeTimerAdd                                      62.79%    78.52ms    12.74
BucketTimerAdd                                   107.28%    45.96ms    21.76
HashTimerAdd                                     107.31%    45.95ms    21.76
WheelTimerAdd                                     1.14K%     4.31ms   232.08
WheelTimer2Add                                    1.16K%     4.26ms   234.74
----------------------------------------------------------------------------
PQTimerDel                                                   6.86ms   145.78
TreeTimerDel                                      28.01%    24.49ms    40.83
BucketTimerDel                                    55.20%    12.43ms    80.47
HashTimerDel                                     121.21%     5.66ms   176.70
WheelTimerDel                                    256.46%     2.67ms   373.86
WheelTimer2Del                                   194.96%     3.52ms   284.20
----------------------------------------------------------------------------
PQTimerTick                                                 36.76ms    27.20
TreeTimerTick                                     80.13%    45.88ms    21.80
BucketTimerTick                                   96.16%    38.23ms    26.16
HashTimerTick                                     98.35%    37.38ms    26.75
WheelTimerTick                                   103.57%    35.50ms    28.17
WheelTimer2Tick                                  102.47%    35.88ms    27.87
============================================================================
```

```
i7 4.2GHz 8700 Windows 10 Visual C++ 2019 /O2
============================================================================
PQTimerAdd                                                  23.18ms    43.14
TreeTimerAdd                                      43.34%    53.49ms    18.70
BucketTimerAdd                                   105.56%    21.96ms    45.53
WheelTimerAdd                                    173.30%    13.38ms    74.75
WheelTimer2Add                                   173.67%    13.35ms    74.91
----------------------------------------------------------------------------
PQTimerDel                                                  18.66ms    53.60
TreeTimerDel                                      75.79%    24.62ms    40.62
BucketTimerDel                                    89.46%    20.86ms    47.95
WheelTimerDel                                    284.58%     6.56ms   152.53
WheelTimer2Del                                   198.38%     9.40ms   106.33
----------------------------------------------------------------------------
PQTimerTick                                                  3.88ms   257.75
TreeTimerTick                                     30.92%    12.55ms    79.70
BucketTimerTick                                   67.40%     5.76ms   173.73
WheelTimerTick                                    85.37%     4.54ms   220.05
WheelTimer2Tick                                   82.83%     4.68ms   213.48
============================================================================
```

```
Intel(R) Xeon(R) CPU E5-2620 v3 @ 2.40GHz Ubu16.04 GCC 9.3 -O3
============================================================================
PQTimerAdd                                                  14.80ms    67.57
TreeTimerAdd                                      27.21%    54.38ms    18.39
BucketTimerAdd                                    75.18%    19.68ms    50.81
WheelTimerAdd                                    163.94%     9.03ms   110.78
WheelTimer2Add                                   163.96%     9.03ms   110.79
----------------------------------------------------------------------------
PQTimerDel                                                  18.24ms    54.84
TreeTimerDel                                      63.64%    28.66ms    34.90
BucketTimerDel                                   102.33%    17.82ms    56.12
WheelTimerDel                                    378.31%     4.82ms   207.46
WheelTimer2Del                                   289.16%     6.31ms   158.57
----------------------------------------------------------------------------
PQTimerTick                                                  2.86ms   349.91
TreeTimerTick                                     23.88%    11.97ms    83.55
BucketTimerTick                                   61.74%     4.63ms   216.05
WheelTimerTick                                    82.93%     3.45ms   290.17
WheelTimer2Tick                                   84.41%     3.39ms   295.37
============================================================================
```

```
Intel(R) Xeon(R) CPU 8750u 2.80GHz Apple clang++ 11 -O3
============================================================================
PQTimerAdd                                                  15.65ms    63.89
TreeTimerAdd                                      26.39%    59.32ms    16.86
BucketTimerAdd                                    74.58%    20.99ms    47.65
HashTimerAdd                                      74.63%    20.97ms    47.68
WheelTimerAdd                                    126.85%    12.34ms    81.04
WheelTimer2Add                                   121.63%    12.87ms    77.71
----------------------------------------------------------------------------
PQTimerDel                                                  14.15ms    70.67
TreeTimerDel                                      35.06%    40.36ms    24.77
BucketTimerDel                                    83.57%    16.93ms    59.06
HashTimerDel                                     140.39%    10.08ms    99.22
WheelTimerDel                                    235.28%     6.01ms   166.28
WheelTimer2Del                                   201.45%     7.02ms   142.37
----------------------------------------------------------------------------
PQTimerTick                                                  5.40ms   185.22
TreeTimerTick                                     22.15%    24.38ms    41.02
BucketTimerTick                                   70.35%     7.67ms   130.29
HashTimerTick                                    100.15%     5.39ms   185.49
WheelTimerTick                                   105.90%     5.10ms   196.16
WheelTimer2Tick                                  107.29%     5.03ms   198.73
============================================================================
```

```
amd 4.4GHz 5800H Windows 10 Visual C++ 2019 /O2
============================================================================
PQTimerAdd                                                  14.75ms    67.80
TreeTimerAdd                                      35.86%    41.13ms    24.31
BucketTimerAdd                                    85.75%    17.20ms    58.14
HashTimerAdd                                     116.45%    12.67ms    78.95
WheelTimerAdd                                    174.92%     8.43ms   118.59
WheelTimer2Add                                   171.98%     8.58ms   116.59
----------------------------------------------------------------------------
PQTimerDel                                                   9.30ms   107.53
TreeTimerDel                                      55.61%    16.72ms    59.80
BucketTimerDel                                    66.97%    13.89ms    72.02
HashTimerDel                                     161.47%     5.76ms   173.64
WheelTimerDel                                    238.41%     3.90ms   256.38
WheelTimer2Del                                   143.18%     6.49ms   153.97
----------------------------------------------------------------------------
PQTimerTick                                                  3.11ms   321.20
TreeTimerTick                                     39.97%     7.79ms   128.37
BucketTimerTick                                   79.57%     3.91ms   255.59
HashTimerTick                                    102.85%     3.03ms   330.37
WheelTimerTick                                   100.86%     3.09ms   323.97
WheelTimer2Tick                                  100.73%     3.09ms   323.55
============================================================================
```
# 结论

* Windows和Linux, Mac平台有一些差异
* 相比之下，TreeTimer因为动态内存,O(lgn)导致性能最差
* Hash桶实现的定时器对于数量不是巨大(<1万), 性能不错适合于客户端使用
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：红黑树 < 哈希桶 < 时间轮 < 最小堆


## TO-DO

* 非STL版的红黑树实现；
* 时间轮的删除实现；
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据

