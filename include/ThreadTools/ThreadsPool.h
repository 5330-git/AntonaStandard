#ifndef THREADSPOOL_H
#define THREADSPOOL_H

#include <mutex>
#include <thread>
#include <vector>
#include "Utilities/ThreadSafeQueue.h"
#include <future>
#include <atomic>
#include <condition_variable>
#include <cassert>
#include <chrono>
#include <functional>
#include "TestingSupport/TestingMessageMacro.h"

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
        TESTING_MESSAGE
    private:
        // 数据成员
        std::atomic<bool> is_shutdown;
            // 描述线程数量
            // 无条件竞争的对象
        int max_thread_num;           // 最大线程数
        int min_thread_num;           // 最小线程数（核心线程数）        
            // 有条件竞争的对象, manager 访问时需要进行快照
        std::atomic<int> live_thread_num;                // 存活线程数（等待任务的线程数 + 正在工作的线程数）  
                                            // 可由 manager_thread 和 thr_pool[i] 修改
        
        std::atomic<int> busy_thread_num;
            // 锁与条件变量，用于唤醒线程，以及创建快照（事务）
        std::mutex mtx_thr_pool;
        std::condition_variable cv_thr_pool;
            // 其它数据
        std::vector<std::thread> thr_pool;   // 解决任务的线程  (线程池本体) 
        std::thread manager_thread;         // 用于管理线程
        AntonaStandard::Utilities::ThreadSafeQueue<std::function<void()>> tasks;    // 存储任务的队列
    private:
        void work();
        void manage();
    protected:
        virtual std::chrono::milliseconds get_manager_waiting_period();
        virtual int get_add_thread_num( int live_thread_num_snapshot, 
                                        int busy_thread_num_snapshot,
                                        int tasks_num_snap);

        virtual bool is_need_motivate(  int live_thread_num_snapshot, 
                                        int busy_thread_num_snapshot,
                                        int tasks_num_snapshot);

        virtual bool is_neeed_exit( int live_thread_num_snapshot,
                                    int busy_thread_num_snapshot,
                                    int tasks_num_snapshot);

    public:
        ThreadsPool(int min_thread_nums = 1, int max_thread_nums = 2):
            is_shutdown(false),
            max_thread_num(max_thread_nums),
            min_thread_num(min_thread_nums),
            live_thread_num(min_thread_num),
            busy_thread_num(0),
            mtx_thr_pool(),
            cv_thr_pool(),
            thr_pool(std::vector<std::thread>(max_thread_num)){
                // 动态断言，检查传入参数是否合法
                assert(min_thread_nums <= max_thread_nums);
                assert(min_thread_nums > 0);
                assert(max_thread_nums > 0);
        };
        // 删除拷贝构造，移动构造，拷贝赋值移动赋值
        ThreadsPool(const ThreadsPool&) = delete;
        ThreadsPool& operator=(const ThreadsPool&) = delete;
        ThreadsPool(ThreadsPool&&) = delete;
        ThreadsPool& operator=(ThreadsPool&&) = delete;
        virtual ~ThreadsPool();
        void lauch();
        void shutdown();
        template<typename type_Func,typename... type_Args>
        auto submit(type_Func&& func,type_Args&&... args)->std::future<decltype(func(args...))>{
            using Returned_type = decltype(func(args...));
            // 检查线程是否关闭
            if(this->is_shutdown.load(std::memory_order_relaxed)){
                std::promise<Returned_type> promise;
                promise.set_exception(std::make_exception_ptr(std::runtime_error("thread pool is shutdown")));
                return promise.get_future();
            }
            // 将func(args...) 包装为 Returned_type(void) 形式的function
            std::function<Returned_type(void)> task = std::bind(func,args...);
            // 包装为packaged_task,为了让任务队列中的packaged_task 与这里的packaged_task可以共享，需要用shared_ptr包装
            auto p_task = std::make_shared<std::packaged_task<Returned_type(void)>>(task);
            // 使用lambda 表达式包装，然后存入任务队列中
            auto submit_task = [p_task](){
                return (*p_task)();
            };
            // 提交任务
            this->tasks.push(submit_task);
            // 通知一个线程取任务
            this->cv_thr_pool.notify_one();
            // 返回包装任务的future
            return p_task->get_future();

        }
    };
}

#endif