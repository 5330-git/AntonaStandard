#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include "ThreadTools/ThreadsPool.h"
#include <CPS/Macros.h>
using namespace AntonaStandard::ThreadTools;

#include <mutex>
using namespace std;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution <> dist(-1000,3000);      // 任务的复杂度分布较广
std::uniform_int_distribution <> dist2(1,10);
int multi(int a,int b){
    // 假设任务很复杂
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)+1000));
    return a*b;
}
int main(){
    ThreadsPool pool(4,20);
    std::vector<std::future<int>> result; 
    std::vector<std::vector<int>> args;
    pool.lauch();
    // 提交100个任务
    for(int i = 0;i<=200;++i){
        args.push_back({dist2(gen),dist2(gen)});
        result.push_back(pool.submit(multi,args.back()[0],args.back()[1]));
        
    }
        
    
    for(int i = 0;i<200;++i){
        cout<<"Task "<<i <<" : "<< args[i][0]<<" * "<<args[i][1] <<" = ";
        // 设置等待时间，防止超时死锁
        auto status = result[i].wait_for(std::chrono::seconds(5));
        if(status == std::future_status::timeout){
            cout<<"time out"<<endl;
        }
        else{
            cout<<result[i].get()<<endl;
        }
        
    }
    cout<<"All tasks done!"<<endl;
    
    pool.shutdown();
    cout<<"ThreadsPool shutdown successfully!"<<endl;
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
    return 0;
}