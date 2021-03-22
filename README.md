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
PQTimerAdd                                                  25.04ms    39.94
TreeTimerAdd                                      79.29%    31.57ms    31.67
HashTimerAdd                                     113.87%    21.99ms    45.48
WheelTimerAdd                                    652.96%     3.83ms   260.80
WheelTimer2Add                                    1.35K%     1.86ms   538.30
----------------------------------------------------------------------------
PQTimerDel                                                   4.44ms   225.30
TreeTimerDel                                     37.06m%     11.98s   83.49m
HashTimerDel                                     118.63%     3.74ms   267.26
WheelTimerDel                                    216.82%     2.05ms   488.50
WheelTimer2Del                                   293.75%     1.51ms   661.81
----------------------------------------------------------------------------
PQTimerTick                                                 17.68ms    56.58
TreeTimerTick                                     91.26%    19.37ms    51.63
HashTimerTick                                     94.69%    18.67ms    53.57
WheelTimerTick                                    91.26%    19.37ms    51.63
WheelTimer2Tick                                  102.30%    17.28ms    57.88
============================================================================
```

```
i5 3.4GHz Windows 10 Visual C++ 19 /O2
============================================================================
PQTimerAdd                                                  25.04ms    39.94
TreeTimerAdd                                      79.29%    31.57ms    31.67
HashTimerAdd                                     113.87%    21.99ms    45.48
WheelTimerAdd                                    652.96%     3.83ms   260.80
WheelTimer2Add                                    1.35K%     1.86ms   538.30
----------------------------------------------------------------------------
PQTimerDel                                                   4.44ms   225.30
TreeTimerDel                                     37.06m%     11.98s   83.49m
HashTimerDel                                     118.63%     3.74ms   267.26
WheelTimerDel                                    216.82%     2.05ms   488.50
WheelTimer2Del                                   293.75%     1.51ms   661.81
----------------------------------------------------------------------------
PQTimerTick                                                 17.68ms    56.58
TreeTimerTick                                     91.26%    19.37ms    51.63
HashTimerTick                                     94.69%    18.67ms    53.57
WheelTimerTick                                    91.26%    19.37ms    51.63
WheelTimer2Tick                                  102.30%    17.28ms    57.88
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
