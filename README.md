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
PQTimerAdd                                                  52.88ms    18.91
TreeTimerAdd                                      74.47%    71.00ms    14.08
HashTimerAdd                                     128.25%    41.23ms    24.25
WheelTimerAdd                                    567.86%     9.31ms   107.40
WheelTimer2Add                                    1.26K%     4.18ms   239.07
----------------------------------------------------------------------------
PQTimerDel                                                  14.79ms    67.59
TreeTimerDel                                     19.23m%    1.28min   13.00m
HashTimerDel                                     387.48%     3.82ms   261.91
WheelTimerDel                                    219.33%     6.75ms   148.25
WheelTimer2Del                                   450.62%     3.28ms   304.59
----------------------------------------------------------------------------
PQTimerTick                                                 37.56ms    26.63
TreeTimerTick                                     85.84%    43.75ms    22.85
WheelTimerTick                                    87.77%    42.79ms    23.37
WheelTimer2Tick                                   98.82%    38.01ms    26.31
============================================================================
```

```
i5 3.4GHz Windows 10 Visual C++ 19 /O2
============================================================================
PQTimerAdd                                                  23.12ms    43.25
TreeTimerAdd                                      61.32%    37.71ms    26.52
HashTimerAdd                                     208.95%    11.07ms    90.37
WheelTimerAdd                                     87.27%    26.49ms    37.74
WheelTimer2Add                                   215.08%    10.75ms    93.02
----------------------------------------------------------------------------
PQTimerDel                                                  18.63ms    53.67
TreeTimerDel                                     39.71m%     46.92s   21.31m
HashTimerDel                                     249.00%     7.48ms   133.64
WheelTimerDel                                    137.52%    13.55ms    73.81
WheelTimer2Del                                   369.01%     5.05ms   198.05
----------------------------------------------------------------------------
PQTimerTick                                                  2.27ms   440.33
TreeTimerTick                                     17.37%    13.08ms    76.47
WheelTimerTick                                    22.32%    10.17ms    98.29
WheelTimer2Tick                                   84.54%     2.69ms   372.25
============================================================================
```


# 结论

* Windows和Linux两个平台有一些差异；
* 相比之下，最小堆PerTick()性能消耗最少；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 非STL版的红黑树实现；
* 时间轮的删除实现；
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
