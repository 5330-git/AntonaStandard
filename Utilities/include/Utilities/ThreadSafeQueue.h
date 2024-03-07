/**
 * @file ThreadSafeQueue.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 实现线程安全的队列
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILITIES_THREADSAFEQUEUE_H
#define UTILITIES_THREADSAFEQUEUE_H

#include <deque>
#include <mutex>
#include <TestingSupport/TestingMessageMacro.h>
#include <memory>

namespace AntonaStandard{
    namespace Utilities{
        template<typename type_Ele>
        class ThreadSafeQueue;
    }
}

namespace AntonaStandard::Utilities{
    /**
     * @brief 线程安全的队列
     * @details
     *      该类是 std::queue 容器的封装，主要对一些接口增加线程安全的机制
     * @tparam type_Ele 
     */
    template <typename type_Ele>
    class ThreadSafeQueue{
        TESTING_MESSAGE
    private:
        /// @brief 实现队列基本功能的队列容器
        std::deque<type_Ele> data_que;              // implement of ThreadSafeQueue 
        /// @brief 维护数据队列的互斥锁
        mutable std::mutex data_que_mtx;            // 用于data_que 的同步操作
    public:
        ThreadSafeQueue() = default;
        // 应当禁用拷贝构造和拷贝赋值,以及移动拷贝和移动赋值
        ThreadSafeQueue(const ThreadSafeQueue& rhs) = delete;
        ThreadSafeQueue& operator=(const ThreadSafeQueue& rhs) = delete;
        ThreadSafeQueue(ThreadSafeQueue&& rhs) = delete;
        ThreadSafeQueue& operator=(ThreadSafeQueue& rhs) = delete;
        ~ThreadSafeQueue() = default;
        
        /**
         * @brief 保障线程安全和异常安全的push操作
         * 
         * @param value 
         */
        inline void push(type_Ele value) {
            // 上锁
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            // 存放数据
            this->data_que.push_back(std::move(value));
        }

        /**
         * @brief 保障线程安全和异常安全的pop操作
         * @details
         *      函数的返回值赋值和pop操作不是原子的，可能存在条件竞争的问题，因此将pop结果的赋值移动到函数体内
         * @param target 
         * @return true 
         * @return false 
         */
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
        /**
         * @brief pop操作
         * 
         * @return std::shared_ptr<type_Ele> 
         */
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
        /// @brief 获取队列大小
        /// @return 
        inline size_t size()const noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            return this->data_que.size();
        }
        /// @brief 判断队列是否为空
        /// @return 
        inline bool empty()const noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            return this->data_que.empty();
        }
        /// @brief 清空队列
        inline void clear()noexcept{
            std::lock_guard<std::mutex> lck(this->data_que_mtx);
            this->data_que.clear();
        }
    };
    
}
#endif
