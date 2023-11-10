#include <future>
#include <gtest/gtest.h>
#include <Utilities/ThreadSafeQueue.h>
#include <chrono>
#include <random>
using AntonaStandard::Utilities::ThreadSafeQueue;
std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis_data(10000000, 99999999);


void push_thread(ThreadSafeQueue<int> &queue,int data,int times){
    for(int i = 0; i < times; i++){
        queue.push(data);
    }
}
int pop_thread_1(ThreadSafeQueue<int> &queue,int data,int times){
    int poped_count = 0;
    for(int i = 0; i < times; i++){
        int value = 0;
        if(queue.pop(value)){
            ++poped_count;
            EXPECT_EQ(value,data)<<"times: "<<i;
        }
    }
    return poped_count;
}
int pop_thread_2(ThreadSafeQueue<int> &queue,int data,int times){
    int poped_count = 0;
    for(int i = 0; i < times; i++){
        auto result = queue.pop();
        if(result){
            int result_value = 0;
            ++poped_count;
            result_value = *(result.get());
            EXPECT_EQ(result_value,data)<<"times: "<<i;
        }
    }
    return poped_count;
}

TEST(Test_ThreadSafeQueue, Empty_Pop){

    // 生成插入数据
    int data = dis_data(gen);
    ThreadSafeQueue<int> queue;
    // 单线程插入与弹出测试,
    int test_time = 1000000;
    int poped_count = pop_thread_1(queue,data,test_time);
    int poped_count_2 = pop_thread_2(queue,data,test_time);
    EXPECT_EQ(poped_count,poped_count_2);
    EXPECT_EQ(poped_count,0);
}

TEST(Test_ThreadSafeQueue, SingleThread_PopPush){

    // 生成插入数据
    int data = dis_data(gen);
    ThreadSafeQueue<int> queue;
    // 单线程插入与弹出测试,
    int test_time = 1000000;
    push_thread(queue,data,test_time);
    int poped_count = pop_thread_1(queue,data,test_time);
    EXPECT_EQ(poped_count,test_time);

}

TEST(Test_ThreadSafeQueue, MultiThread_PopPush){
    
    // 生成插入数据
    int data = dis_data(gen);
    ThreadSafeQueue<int> queue;
    // 多线程插入与弹出测试,
    int test_time = 1000000;
    auto push_thread_fu = std::async(std::launch::async,push_thread,std::ref(queue),data,test_time);
    auto pop_thread_1_fu = std::async(std::launch::async,pop_thread_1,std::ref(queue),data,test_time);
    auto pop_thread_2_fu = std::async(std::launch::async,pop_thread_2,std::ref(queue),data,test_time);

    int actual_pop_count_1 = pop_thread_1_fu.get();
    int actual_pop_count_2 = pop_thread_2_fu.get();

    int remain = queue.size();
    
    ASSERT_EQ(actual_pop_count_1+actual_pop_count_2+remain,test_time);
}

TEST(Test_ThreadSafeQueue,MultiThread_Clear){
    // 生成插入数据
    int data = dis_data(gen);
    ThreadSafeQueue<int> queue;
    // 多线程插入与弹出测试,
    int test_time = 1000000;
    auto push_thread_fu = std::async(std::launch::async,push_thread,std::ref(queue),data,test_time);
    auto pop_thread_1_fu = std::async(std::launch::async,pop_thread_1,std::ref(queue),data,test_time);
    auto pop_thread_2_fu = std::async(std::launch::async,pop_thread_2,std::ref(queue),data,test_time);

    int actual_pop_count_1 = pop_thread_1_fu.get();
    queue.clear();
    int actual_pop_count_2 = pop_thread_2_fu.get();
    
}
