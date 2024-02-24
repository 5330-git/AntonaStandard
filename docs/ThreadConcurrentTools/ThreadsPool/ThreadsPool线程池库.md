# TreadPool 线程池库

## 目录

[toc]

## 项目版本

| 版本号  | 版本描述                                       | 时间     |
| ------- | ---------------------------------------------- | -------- |
| v-1.0.0 | 初步实现：动态调整，可接受任意参数任务的线程池 | 2023/8/8 |

## 项目目的

- 实现线程池，解决频繁创建线程造成的系同效率的大大降低
- **动态管理**需要的线程数量来匹配任务数量，有管理者每3秒检查一次：
  - 设置 核心线程数 与 当前线程数
  - 当当前线程数大于核心线程数，在等待任务若干秒后，将自己从线程**自动结束** 
  - 当当前线程数小于核心线程数，自动补齐。
  - 当任务数量是当前线程两倍以上，则增加一个线程

- 并且我们的线程池支持提交**任意参数的任务**进行执行

### 项目原理架构

- 本质是一个 **生产者——消费者模型** 

  - 使用条件变量 `std::condition_variable` 来实现线程同步

- ```mermaid
  graph TB
  main(客户端)
  
  Thread(存活线程)
  
  Tasks[任务队列]
  Threads[存活线程表]
  DeadThreads[死亡线程表]
  Manager(管理者线程)
  main--锁定线程池-->Tasks
  Threads--存储-->Thread
  Thread--锁定线程池-->Tasks
  Thread--超时自杀-->DeadThreads
  
  main--线程池析构时首先销毁管理者-->Manager
  main--线程池析构时遍历销毁-->Threads
  Manager--锁定线程池, 按照算法添加线程-->Threads
  Manager--锁定线程池, 清空死亡线程-->DeadThreads
  ```

- 没有复杂的继承关系

### 使用说明

#### `AntonaStandard::ThreadTools::ThreadPool::submit()` 

- **该成员函数是一个可变参数函数，可以接收一个返回值为void的任意参数的函数指针** 

```cpp
#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include "ThreadTools/ThreadsPool.h"
using namespace AntonaStandard::ThreadTools;

#include <mutex>
using namespace std;
std::mutex protect_bash_mtx;
void task(int code){
    this_thread::sleep_for(std::chrono::seconds(1));
    unique_lock<mutex> lck(protect_bash_mtx);
    cout<<this_thread::get_id() <<" Executing Task:"<<code<<endl;
}
int main(){
    ThreadsPool th(10);				// 设置核心线程数量为10
    for(int i = 0;i<60;++i){		// 提交六十个任务
        th.submit(task,i);
    }
    
    return 0;
}
```

