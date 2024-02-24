#include <gtest/gtest.h>
#include <iostream>
#include <functional>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include "ThreadTools/ThreadsPool.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
using AntonaStandard::ThreadTools::ThreadsPool;

#include <mutex>
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution <> dist(-1000,5000); 
std::uniform_int_distribution <> dist1(-100,1200); 
std::uniform_int_distribution <> dist2(1,10);
int complex_task(int a,int b){
    // 假设任务很复杂
    std::this_thread::sleep_for(std::chrono::milliseconds(dist(gen)+1000));
    return a*b;
}
int midium_task(int a,int b){
    std::this_thread::sleep_for(std::chrono::milliseconds(dist1(gen)+100));
    return a*b;
}
int simple_task(int a,int b){
    return a*b;
}

TEST(Test_ThreadsPool, Interrupted_Thread_Pool){
    auto pool_ptr = std::make_shared<ThreadsPool>(4,60);
    std::vector<std::future<int>> result; 
    // 添加任务1000 个
    for(int i = 0;i<=1000;++i){
        result.push_back(pool_ptr->submit(complex_task,dist1(gen),dist1(gen)));
    }
    
    // pool.lauch();
    pool_ptr.reset();                // 立刻打断，所有任务销毁(析构销毁)

    for(int i = 0;i<1000;++i){
        EXPECT_THROW(result[i].get(),std::future_error);
    }
}


TEST(TEST_ThreadsPoolTest, Complax_task){
    ThreadsPool pool(4,60);
    std::vector<std::future<int>> result; 
    std::vector<std::vector<int>> args;
    
    // 提交10个复杂任务
    int task_count = 10;
    int time_out_task_count = 0;
    int finished_task_count = 0;
    for(int i = 0;i<=task_count;++i){
        args.push_back({dist2(gen),dist2(gen)});
        result.push_back(pool.submit(complex_task,args.back()[0],args.back()[1]));
        
    }
    pool.lauch();
    
    for(int i = 0;i<task_count;++i){
        // 设置等待时间，防止超时死锁
        auto status = result[i].wait_for(std::chrono::seconds(5));
        if(status == std::future_status::timeout){
            ++time_out_task_count;
        }
        else{
            EXPECT_EQ(args[i][0]*args[i][1],result[i].get());
            ++finished_task_count;
        }
        
        std::cout<<"finished task count: "<<finished_task_count<<std::endl;
        
    }
    EXPECT_EQ(task_count,time_out_task_count+finished_task_count);
    
    pool.shutdown();
    // 再提交任务会触发异常
    auto invalid_task_result = pool.submit(complex_task,1,1);
    EXPECT_THROW(invalid_task_result.get(),std::runtime_error);
}
TEST(TEST_ThreadsPoolTest, Midum_task){
    ThreadsPool pool(15,30);
    std::vector<std::future<int>> result; 
    std::vector<std::vector<int>> args;
    
    // 提交1000个中等任务
    int task_count = 1000;
    int time_out_task_count = 0;
    int finished_task_count = 0;
    for(int i = 0;i<=task_count;++i){
        args.push_back({dist2(gen),dist2(gen)});
        result.push_back(pool.submit(midium_task,args.back()[0],args.back()[1]));
        
    }
    pool.lauch();
    
    for(int i = 0;i<task_count;++i){
        // 设置等待时间，防止超时死锁
        auto status = result[i].wait_for(std::chrono::seconds(1));
        if(status == std::future_status::timeout){
            ++time_out_task_count;
        }
        else{
            EXPECT_EQ(args[i][0]*args[i][1],result[i].get());
            ++finished_task_count;
        }
        if(i%100 == 0){
            std::cout<<"finished task count: "<<finished_task_count<<std::endl;
        }
    }
    EXPECT_EQ(task_count,time_out_task_count+finished_task_count);
    
    pool.shutdown();
}

TEST(Test_ThreadsPool , Simple_task){
    ThreadsPool pool(4,60);
    std::vector<std::future<int>> result;
    std::vector<std::vector<int>> args;

    // 提交100000个简单任务
    int task_count = 100000;
    int finished_task_count = 0;
    for(int i = 0;i<=task_count;++i){
        args.push_back({dist2(gen),dist2(gen)});
        result.push_back(pool.submit(simple_task,args.back()[0],args.back()[1]));
    }
    pool.lauch();
    for(int i = 0;i<task_count;++i){
        EXPECT_EQ(args[i][0]*args[i][1],result[i].get());
        ++finished_task_count;
        if(i%10000 == 0){
            std::cout<<"finished task count: "<<finished_task_count<<std::endl;
        }
    }
    EXPECT_EQ(task_count,finished_task_count);
    
    pool.shutdown();
}

TEST(TEST_ThreadsPoolTest, BadArguments){
    // 传入错误参数触发中断
    EXPECT_DEATH(ThreadsPool(10,1),".*");
    EXPECT_DEATH(ThreadsPool(-1,10),".*");
    EXPECT_DEATH(ThreadsPool(-1,0),".*");
}

