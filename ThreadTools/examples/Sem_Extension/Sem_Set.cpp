#include <iostream>
#include <thread>
#include <semaphore>
#include <mutex>
#include <vector>
#include <chrono>
#include <ThreadTools/Sem_Extension.h>
#include <CPS/Macros.h>
using namespace std;

AntonaStandard::ThreadTools::Sem_Set_Acquirer sem_ac;
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
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}
