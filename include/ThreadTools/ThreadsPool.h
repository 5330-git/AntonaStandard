#ifndef THREADSPOOL_H
#define THREADSPOOL_H

#include <mutex>
#include <thread>
#include <vector>
#include <queue>
#include <condition_variable>
#include <chrono>
#include <map>
#include <functional>

#define AntonaStandard_ThreadTools_ThreadsPool_VERSION "1.0.0"
#define AntonaStandard_ThreadTools_ThreadsPool_EDIT_TIME "2023/8/8"
#define AntonaStandard_ThreadTools_ThreadsPool_AUTHOR "Anton"
/*
*   Decoded by UTF-8
*   2023/8/8 v-1.0.0 初步实现：动态调整，可接受任意参数任务的线程池
*/
namespace AntonaStandard{
    namespace ThreadTools{
        class ThreadsPool;                      // 线程池类
    }
}
// define
namespace AntonaStandard::ThreadTools{
    
    class ThreadsPool{
    public:
        void work();
        virtual void stratagy();
        ThreadsPool(int coreC = 0);
        template<typename Func_type,typename... Args>
        void submit(Func_type&& func,Args&&... args){
            std::function<void()> task = std::bind(std::forward<Func_type>(func),std::forward<Args>(args)...);
            // 将任务放入任务队列
            {
                std::unique_lock<std::mutex> lck_pool(this->protect_pool_mtx);
                this->tasks.push(std::move(task));
            }
            
            // 唤醒一个线程
            this->task_queue_cv.notify_one();
        }
        ~ThreadsPool();
    protected:
        
        std::mutex protect_pool_mtx;
        int max_seconds = 5;
        bool stop;
        int coreCount = 0;
        std::map<std::thread::id,std::thread*> threads;
        std::map<std::thread::id,std::thread*> dead_threads;
        
        std::condition_variable task_queue_cv;
        std::queue<typename std::function<void()>> tasks;
        std::thread* manager_ptr;
    };
}

#endif