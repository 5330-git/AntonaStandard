#ifndef UTILITIES_THREADSAFEQUEUE_H
#define UTILITIES_THREADSAFEQUEUE_H

#include <deque>
#include <mutex>
#include "TestingSupport/TestingMessageMacro.h"
#include <memory>

namespace AntonaStandard{
    namespace Utilities{
        template<typename type_Ele>
        class ThreadSafeQueue;
    }
}

namespace AntonaStandard::Utilities{
    template <typename type_Ele>
    class ThreadSafeQueue{
        TESTING_MESSAGE
    private:
        std::deque<type_Ele> data_que;              // implement of ThreadSafeQueue 
        mutable std::mutex data_que_mtx;            // 用于data_que 的同步操作
    public:
        ThreadSafeQueue() = default;
        // 应当禁用拷贝构造和拷贝赋值,以及移动拷贝和移动赋值
        ThreadSafeQueue(const ThreadSafeQueue& rhs) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue& rhs) = delete;
        ThreadSafeQueue(ThreadSafeQueue&& rhs) = delete;
        ThreadSafeQueue& operator=(ThreadSafeQueue& rhs) = delete;
        ~ThreadSafeQueue() = default;
        
        // 保证线程安全和异常安全的push与pop操作
        inline void push(type_Ele value) {
            // 上锁
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            // 存放数据
            this->data_que.push_back(std::move(value));
        }

        inline bool pop(type_Ele& target){
            // 判断队列是否为空
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            if(this->data_que.empty()){
                return false;
            }
            // 队列不为空，移动拷贝数据
            target = std::move(this->data_que.front());
            this->data_que.pop_front();
            return true;
        }
        inline std::shared_ptr<type_Ele> pop(){
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            // 判断队列是否为空
            if(this->data_que.empty()){
                return nullptr;
            }
            // 使用make_shared 防止拷贝构造时出现异常，从而打断数据指针被存储到shared_ptr 中，从而导致内存泄漏
            std::shared_ptr<type_Ele> temp = std::make_shared<type_Ele>(this->data_que.front());
            this->data_que.pop_front();
            return temp;
        }
        inline size_t size()const noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            return this->data_que.size();
        }
        inline bool empty()const noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            return this->data_que.empty();
        }
        inline void clear()noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            this->data_que.clear();
        }
    };
    
}
#endif