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
哈希   | O(1)     | O(1)     | O(n)     | src/HashTimer.h
```

最小堆和红黑树的Cancel均使用for遍历找到id做删除，所以都是O(N)复杂度。



```
AMD 1700 3.0GHz Ubu18.04 GCC 9.3 -O3
============================================================================
PQTimerAdd                                                  23.56ms    42.45
TreeTimerAdd                                      78.96%    29.84ms    33.52
HashTimerAdd                                     111.48%    21.13ms    47.32
WheelTimerAdd                                     1.19K%     1.98ms   505.20
WheelTimer2Add                                    1.21K%     1.95ms   512.95
----------------------------------------------------------------------------
PQTimerDel                                                   4.43ms   225.63
TreeTimerDel                                      75.05%     5.91ms   169.34
HashTimerDel                                     132.09%     3.36ms   298.04
WheelTimerDel                                    416.50%     1.06ms   939.76
WheelTimer2Del                                   321.81%     1.38ms   726.11
----------------------------------------------------------------------------
PQTimerTick                                                 16.38ms    61.03
TreeTimerTick                                     89.46%    18.32ms    54.60
HashTimerTick                                     98.03%    16.71ms    59.83
WheelTimerTick                                    98.64%    16.61ms    60.20
WheelTimer2Tick                                   98.47%    16.64ms    60.10
============================================================================
```

```
i7 4.2GHz 8700 Windows 10 Visual C++ 2019 /O2
============================================================================
PQTimerAdd                                                  23.18ms    43.14
TreeTimerAdd                                      43.34%    53.49ms    18.70
HashTimerAdd                                     105.56%    21.96ms    45.53
WheelTimerAdd                                    173.30%    13.38ms    74.75
WheelTimer2Add                                   173.67%    13.35ms    74.91
----------------------------------------------------------------------------
PQTimerDel                                                  18.66ms    53.60
TreeTimerDel                                      75.79%    24.62ms    40.62
HashTimerDel                                      89.46%    20.86ms    47.95
WheelTimerDel                                    284.58%     6.56ms   152.53
WheelTimer2Del                                   198.38%     9.40ms   106.33
----------------------------------------------------------------------------
PQTimerTick                                                  3.88ms   257.75
TreeTimerTick                                     30.92%    12.55ms    79.70
HashTimerTick                                     67.40%     5.76ms   173.73
WheelTimerTick                                    85.37%     4.54ms   220.05
WheelTimer2Tick                                   82.83%     4.68ms   213.48
============================================================================
```

```
Intel(R) Xeon(R) CPU E5-2620 v3 @ 2.40GHz Ubu16.04 GCC 9.3 -O3
/root/timer-benchmarks/test/BenchTimer.cpp       relative  time/iter  iters/s
============================================================================
/root/timer-benchmarks/test/BenchTimer.cpp       relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  14.80ms    67.57
TreeTimerAdd                                      27.21%    54.38ms    18.39
HashTimerAdd                                      75.18%    19.68ms    50.81
WheelTimerAdd                                    163.94%     9.03ms   110.78
WheelTimer2Add                                   163.96%     9.03ms   110.79
----------------------------------------------------------------------------
PQTimerDel                                                  18.24ms    54.84
TreeTimerDel                                      63.64%    28.66ms    34.90
HashTimerDel                                     102.33%    17.82ms    56.12
WheelTimerDel                                    378.31%     4.82ms   207.46
WheelTimer2Del                                   289.16%     6.31ms   158.57
----------------------------------------------------------------------------
PQTimerTick                                                  2.86ms   349.91
TreeTimerTick                                     23.88%    11.97ms    83.55
HashTimerTick                                     61.74%     4.63ms   216.05
WheelTimerTick                                    82.93%     3.45ms   290.17
WheelTimer2Tick                                   84.41%     3.39ms   295.37
============================================================================
```

# 结论

* Windows和Linux两个平台有一些差异；
* 相比之下，改进后的WheelTimer2性能消耗最少；
* Hash桶实现的定时器对于数量不是巨大(>10万), 性能还不错适合于客户端使用
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：哈希桶 < 最小堆 < 时间轮 < 红黑树；


## TO-DO

* 非STL版的红黑树实现；
* 时间轮的删除实现；
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
