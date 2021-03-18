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
AMD 1700 3.0GHz Ubu16.04 GCC 9.3
============================================================================
PQTimerAdd                                                  25.01ms    39.98
TreeTimerAdd                                      80.41%    31.11ms    32.15
HashTimerAdd                                     126.50%    19.77ms    50.58
WheelTimerAdd                                    645.99%     3.87ms   258.28
WheelTimer2Add                                    1.55K%     1.61ms   621.35
----------------------------------------------------------------------------
PQTimerDel                                                   4.85ms   206.04
TreeTimerDel                                     38.86m%     12.49s   80.07m
WheelTimerDel                                    238.37%     2.04ms   491.14
WheelTimer2Del                                   315.01%     1.54ms   649.06
HashTimerDel                                     316.72%     1.53ms   652.57
----------------------------------------------------------------------------
PQTimerTick                                                 18.00ms    55.54
TreeTimerTick                                     89.33%    20.15ms    49.62
WheelTimerTick                                    94.94%    18.96ms    52.73
WheelTimer2Tick                                  102.62%    17.54ms    57.00
============================================================================



```

```
i5 3.4GHz Windows 10 Visual C++ 15
============================================================================
test\benchtimer.cpp                             relative  time/iter  iters/s
============================================================================
PQTimerAdd                                                  12.27ms    81.51
TreeTimerAdd                                      54.89%    22.35ms    44.74
WheelTimerAdd                                     70.70%    17.35ms    57.63
----------------------------------------------------------------------------
PQTimerDel                                                 706.29ms     1.42
TreeTimerDel                                       5.12%     13.81s   72.43m
WheelTimerDel                                     8.07K%     8.75ms   114.23
----------------------------------------------------------------------------
PQTimerTick                                                  1.37ms   728.89
TreeTimerTick                                     38.55%     3.56ms   280.97
WheelTimerTick                                    30.36%     4.52ms   221.31
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
