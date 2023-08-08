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
    ThreadsPool th(10);
    for(int i = 0;i<60;++i){
        th.submit(task,i);
    }
    
    return 0;
}