#include <thread>
#include <mutex>
#include <semaphore>
#include <iostream>
#include <vector>
#include <chrono>
#include "ThreadTools/Sem_Extension.h"
#include <CPS/Macros.h>
using namespace std;


vector<binary_semaphore*> chopsticks;          // 五个信号量表示五支筷子,0,1,2，3，4
binary_semaphore usable(1);		// 实现对窗口的互斥输出
AntonaStandard::ThreadTools::And_Sem_Acquirer ac;
void work(int code){
// 每个哲学家需要做的工作：就餐10次
    // 采取策略，使得最多有四个哲学家可以锁定其左边的筷子
    for(int j = 1;j<=10;++j){
        // 使用And信号量
        ac.and_acquire(*chopsticks[code],*chopsticks[(code+1)%5]);
        usable.acquire();
        
        cout<<"Philosopher "<<code<<" took snacks "<<j<<" times"<<endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(100));     // 休眠100毫秒，更容易体现哲学家之间的竞争
        // 释放输出控制台(释放的顺序无所谓)
        usable.release();
        ac.and_release(*chopsticks[code],*chopsticks[(code+1)%5]);
    }    
}
int main(){
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
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}