# Sem_Extension信号量扩展库



## 目录

[toc] 

## 项目版本

| 版本号  | 版本描述                                                     | 时间      |
| ------- | ------------------------------------------------------------ | --------- |
| v-1.0.0 | 初步实现信号量扩展库(Sem_Extension)，实现And信号量请求与释放，以及信号量集的请求与释放 | 2023/4/11 |

## 项目目的

- 基于C++20信号量库 的计数信号量 conting_semaphore 实现**And信号量机制** 和 **信号量集机制** 
- **And信号量**
  - And信号量允许只有满足所有的信号量都可以请求了，才会占用信号量
- **信号量集** 
  - 信号量集是And信号量的超集，在具备了And信号量的性质的前提下还允许指定可发出请求的信号量下限（低于这个下限，不接受请求），以及一次性需要申请的信号量总数（小于这个数，不接受请求）。



## 项目原理

- **可变模板参数**和**引用折叠**。

## 项目依赖

- C++20标准及以上（Sem_Extenstion 要求）

## 平台

- Windows10
- VSCode
- GCC 11.2.0 x86_64-w64-mingw32

## 项目结构

| 类               | 目的                                                         |
| ---------------- | ------------------------------------------------------------ |
| And_Sem_Acquirer | 实现And信号量的请求,其中有多个模板成员，每个对象唯一维护一个互斥锁 |
| Sem_Set_Acquirer | 实现信号量集的请求，其中有多个模板成员，每个对象唯一维护一个互斥锁。 |

## 成员

### And_Sem_Acquirer

#### 私有成员变量

> `std::mutex and_sem_mutex;` 

- 用于and_sem_isAvaliable的递归调用互斥

#### 私有成员函数 and_sem_isAvaliable

**重载版本一** 

> `template<typename... type_Args,long long sem_max_counts>bool and_sem_isAvaliable(std::counting_semaphore<sem_max_counts>& sem, type_Args&... args)`

- 用于递归检查传入的若干信号量是否可以被同时锁定，如果可以则返回true否则返回false

**重载版本二** 

> `template<long long sem_max_counts> bool AntonaStandard::And_Sem_Acquirer::and_sem_isAvaliable(std::counting_semaphore<sem_max_counts> &sem)` 

- 用于参数展开的递归出口

#### 私有成员函数 and_sem_testing

> `template<class... type_Args, long long sem_max_counts> bool AntonaStandard::And_Sem_Acquirer::and_sem_testing(std::counting_semaphore<sem_max_counts> &sem, type_Args &...args)` 

- 使用私有锁 **and_sem_mutex** 用于包装 **and_sem_isAvailable** 使得其递归调用是互斥进行的。



#### 公有成员函数 and_acquire

> `template<class... type_Args, long long sem_max_counts> void AntonaStandard::And_Sem_Acquirer::and_acquire(std::counting_semaphore<sem_max_counts> &sem, type_Args &...args)` 

- 以And 的形式，向传入的信号量发出请求，只有所有的请求都被通过的情况下才可以占用（信号量减一），否则让出时间片，进入自锁



#### 公有成员函数 and_release

**重载版本一** 

> `template<class... type_Args, long long sem_max_counts> void AntonaStandard::And_Sem_Acquirer::and_release(std::counting_semaphore<sem_max_counts> &sem, type_Args &...args)` 

- 释放传入的信号量

**重载版本二** 

> `template<long long sem_max_counts> void AntonaStandard::And_Sem_Acquirer::and_release(std::counting_semaphore<sem_max_counts> &sem)` 

- 是重载版本一展开参数包的递归出口。



### Sem_Set_Acquirer

#### 私有成员变量

#### 私有成员函数 try_acquire_n 

> `template<long long sem_max_counts> void AntonaStandard::And_Sem_Acquirer::and_release(std::counting_semaphore<sem_max_counts> &sem)`  

- 由 **sem_set_isAvaliable** 调用，用来一次性申请若干资源（信号量减n）



#### 私有成员函数 sem_set_isAvaliable 

**重载版本一** 

> `template<class... type_Args, long long sem_max_counts> bool AntonaStandard::Sem_Set_Acquirer::sem_set_isAvaliable(std::counting_semaphore<sem_max_counts> &sem, int ava_least, int ava_counts, type_Args &&...args)` 

- 用于检查传入的信号量是否可以全部都被占用，如果可以则对每个信号量请求 ava-counts个资源，否则不请求，让出时间片进入自锁状态。
- 其中ava_least 表示如果信号量的可用资源小于这个值，那么就不允许占用
- ava_counts 表示一次性需要请求的资源个数



**重载版本二** 

> `template<long long sem_max_counts> bool AntonaStandard::Sem_Set_Acquirer::sem_set_isAvaliable(std::counting_semaphore<sem_max_counts> &sem, int ava_least, int ava_counts)` 

- 为**重载版本一**参数展开的递归出口 



#### 私有成员函数 sem_set_testing 

> `template<class... type_Args, long long sem_max_counts> bool AntonaStandard::Sem_Set_Acquirer::sem_set_testing(std::counting_semaphore<sem_max_counts> &sem, int ava_least, int ava_counts, type_Args &&...args)` 

- 用于包装 **sem_set_isAvaliable** ，使用 私有得互斥锁 sem_set_mutex 实现**sem_set_isAvaliable** 递归展开过程的互斥 



#### 公有成员函数 sem_set_acquire

> `template<class... type_Args, long long sem_max_counts> void AntonaStandard::Sem_Set_Acquirer::sem_set_acquire(std::counting_semaphore<sem_max_counts> &sem, int ava_least, int ava_counts, type_Args &&...args)` 

- 向若干个信号量发出请求，注意传参的格式如下：

  > sem_set_acquire(信号量一, 信号量一的最小可请求数,  信号量一的一次请求数,   信号量二, 信号量二的最小可请求数,  信号量二的一次请求数,.... , 信号量N,  信号量N的最小可请求数,  信号量N的一次请求数,)

- **示例**

  ```cpp
  AntonaStandard::Sem_Set_Acquirer set_ac;
  std::counting_semaphore sem1;
  std::counting_semaphore sem2;
  std::counting_semaphore sem3;
  set_ac.sem_set_acquire(sem1,0,1,sem2,1,2,sem3,2,3);
  ```

#### 公有成员函数 sem_set_release 

**重载版本一**

> `template<class... type_Args, long long sem_max_counts> void AntonaStandard::Sem_Set_Acquirer::sem_set_release(std::counting_semaphore<sem_max_counts> &sem, int ava_counts, type_Args &&...args)` 

- 令传入的每一个信号量都释放 ava_counts个资源 

- 格式如下

  > sem_set_release(信号量一，信号量一需要释放的资源数，信号量一，信号量一需要释放的资源数，……，信号量N，信号量N需要释放的资源数) 

- **示例**

  ```cpp
  AntonaStandard::Sem_Set_Acquirer set_ac;
  std::counting_semaphore sem1;
  std::counting_semaphore sem2;
  std::counting_semaphore sem3;
  set_ac.sem_set_acquire(sem1,1,sem2,2,sem3,3);
  ```

  

**重载版本二** 

> `template<long long sem_max_counts> void AntonaStandard::Sem_Set_Acquirer::sem_set_release(std::counting_semaphore<sem_max_counts> &sem, int ava_counts)` 

- 为 **重载版本一** 参数展开的递归出口



## 相关演示

- 使用 And信号量实现 **哲学家就餐问题**

```cpp
#include <thread>
#include <mutex>
#include <semaphore>
#include <iostream>
#include <vector>
#include <chrono>
#include "Sem_Extension.h"
using namespace std;


vector<binary_semaphore*> chopsticks;          // 五个信号量表示五支筷子,0,1,2，3，4
binary_semaphore usable(1);		// 实现对窗口的互斥输出
AntonaStandard::And_Sem_Acquirer ac;
void work(int code){
// 每个哲学家需要做的工作：就餐10次
    // 采取策略，使得最多有四个哲学家可以锁定其左边的筷子
    for(int j = 1;j<=10;++j){
        // 使用And信号量
        ac.and_acquire(*chopsticks[code],*chopsticks[(code+1)%5]);
        usable.acquire();
        
        cout<<"哲学家 "<<code<<" 就餐第"<<j<<" 次"<<endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));     // 休眠100毫秒，更容易体现哲学家之间的竞争
        // 释放输出控制台(释放的顺序无所谓)
        usable.release();
        ac.and_release(*chopsticks[code],*chopsticks[(code+1)%5]);
    }    
}
int main(){
    system("chcp 65001");
    for(int i = 0;i<5;++i){
        // 创建表示筷子的信号量
        chopsticks.push_back(new binary_semaphore(1));
    }
    vector<thread> threads;
    for(int i = 0;i<5;++i){
        // 创建线程
        threads.emplace_back(work,i);
    }
    // 循环等待线程
    for(int i = 0;i<threads.size();++i){
        threads[i].join();
    }
    for(auto& i:chopsticks){
        // 删除信号量
        delete i;
    }
    cout<<"finished!"<<endl;
    return 0;
}
```



- 使用信号量集解决 **读者与写者问题** 

```cpp
#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>
#include "Sem_Extension.h"
using namespace std;

AntonaStandard::Sem_Set_Acquirer sem_ac;
mutex terminal_mutex;
// 初始化信号量
counting_semaphore sem_L(2);            // 即书上的信号量L,初始化为读者线程的个数(RN)
counting_semaphore sem_mx(1);           // 即书上的 mx,用于写线程
vector<int> resource;
void writer_task(){
    // 执行50次写入操作，每次写入一个数据
    for(int i = 0;i<50;++i){
        sem_ac.sem_set_acquire(sem_mx,1,1,sem_L,2,0); 
        terminal_mutex.lock();
        cout<<"Writer: "<<this_thread::get_id()<<" Write in: "<<i<<endl;
        resource.push_back(i);
        terminal_mutex.unlock();
        sem_ac.sem_set_release(sem_mx,1);
        std::this_thread::sleep_for(std::chrono::milliseconds(30));
    }
}
void reader_task(){
    // 执行100次读取操作,比较容易看出效果
    for(int i = 0;i<50;++i){
        sem_ac.sem_set_acquire(sem_L,1,1);
        sem_ac.sem_set_acquire(sem_mx,1,0);
        // 读取操作：遍历数组resource
        terminal_mutex.lock();
        cout<<"Reader: "<<this_thread::get_id()<<" contents:"<<endl;
        for(auto& i:resource){
            cout<<i<<" ,";
        }
        cout<<endl;
        terminal_mutex.unlock();
        sem_ac.sem_set_release(sem_L,1);
        // 稍微休眠一下，方便看出读写的结果
        std::this_thread::sleep_for(std::chrono::milliseconds(60));
    }
}
int main(){
    
    vector<thread> threads;
    for(int i = 0;i<4;++i){
        if(i<2){
            // 前两个申请为写者
            threads.emplace_back(writer_task);
        }
        else{
            // 后两个申请为读者
            threads.emplace_back(reader_task);
        }
    }
    // 等待线程执行完毕
    for(auto& i:threads){
        i.join();
    }
    cout<<"finished!"<<endl;
    return 0;
}
```

