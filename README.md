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
PQTimerAdd                                                  52.54ms    19.03
TreeTimerAdd                                      72.04%    72.93ms    13.71
HashTimerAdd                                     129.09%    40.70ms    24.57
WheelTimerAdd                                    515.78%    10.19ms    98.17
WheelTimer2Add                                    1.21K%     4.34ms   230.66
----------------------------------------------------------------------------
PQTimerDel                                                  15.03ms    66.53
TreeTimerDel                                     18.79m%    1.33min   12.50m
HashTimerDel                                     484.05%     3.10ms   322.06
WheelTimerDel                                    263.61%     5.70ms   175.40
WheelTimer2Del                                   448.31%     3.35ms   298.28
----------------------------------------------------------------------------
PQTimerTick                                                 33.60ms    29.76
TreeTimerTick                                     86.46%    38.86ms    25.73
WheelTimerTick                                    88.43%    38.00ms    26.32
WheelTimer2Tick                                   98.62%    34.07ms    29.35
============================================================================
```

```
i5 3.4GHz Windows 10 Visual C++ 19 /O2
============================================================================
PQTimerAdd                                                  57.32ms    17.45
TreeTimerAdd                                      55.37%   103.51ms     9.66
HashTimerAdd                                     171.66%    33.39ms    29.95
WheelTimerAdd                                     86.44%    66.31ms    15.08
WheelTimer2Add                                   170.61%    33.60ms    29.77
----------------------------------------------------------------------------
PQTimerDel                                                  55.72ms    17.95
TreeTimerDel                                     19.74m%    4.71min    3.54m
HashTimerDel                                     308.60%    18.06ms    55.38
WheelTimerDel                                    152.45%    36.55ms    27.36
WheelTimer2Del                                   358.98%    15.52ms    64.42
----------------------------------------------------------------------------
PQTimerTick                                                  5.06ms   197.64
TreeTimerTick                                     15.02%    33.69ms    29.68
WheelTimerTick                                    18.91%    26.75ms    37.38
WheelTimer2Tick                                   81.82%     6.18ms   161.72
============================================================================
```


# 结论

* Windows和Linux两个平台有一些差异；
* 相比之下，改进后的WheelTimer2性能消耗最少；
* 最小堆的编码实现最简单，大多数编程语言都可以迅速实现，实现难度：最小堆 < 时间轮 < 红黑树；


## TO-DO

* 非STL版的红黑树实现；
* 时间轮的删除实现；
* 集成[其它benchmark框架](https://github.com/google/benchmark)来跑性能数据
