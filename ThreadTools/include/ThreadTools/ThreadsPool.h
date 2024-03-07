#ifndef THREADTOOLS_THREADSPOOL_H
#define THREADTOOLS_THREADSPOOL_H

/**
 * @file ThreadsPool.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 定义线程池
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include <mutex>
#include <thread>
#include <vector>
#include <Utilities/ThreadSafeQueue.h>
#include <future>
#include <atomic>
#include <condition_variable>
#include <cassert>
#include <chrono>
#include <functional>
#include <TestingSupport/TestingMessageMacro.h>

namespace AntonaStandard{
    
    namespace ThreadTools{
        class ThreadsPool;                      // 线程池类
    }
}
// define
namespace AntonaStandard::ThreadTools{
    
    /**
     * @brief 线程池类
     * @details
     *      线程的创建和销毁的开销比较大，在需要高并发的场景下会需要频繁地使用线程，如果在使用线程时采取创建
     *      这将导致巨大的系统开销。因此线程池的意义在于，提前创建若干的线程，将其阻塞。等到有任务需要执行时
     *      再唤醒，让它们自己去取任务执行。这样可以大大降低创建线程的频率。本项目的线程池的核心思想如下：
     *      - 定义一个调度线程，负责、创建线程、补充死亡线程、监控任务量和线程总数，以及在有任务时唤醒线程进行执行
     *      - 用一个线性表存储工作线程，如果任务队列中有可用任务就将其取出并执行，否则阻塞，等待调度线程唤醒
     *      - 所有任务存储在一个线程安全的队列中，任务是由函数包装器 std::function 包装成void(*)(void) 型函数统一调用的，
     *          并且可以通过STL 的异步框架 std::promise 和 std::future 异步地获得任务执行结构
     *      
     *      调度线程主要是通过，构造时设置的最大线程数，最小线程数，以及运行时的存活线程数，繁忙线程数来决定是否唤醒线程
     *      让多余的线程退出，或让休眠的线程工作，甚至判断是否需要添加线程以及添加多少。用户可以通过重写 is_needed_exit() 
     *      和 is_needed_motivate() 还有 get_add_thread_num()
     *      来告诉工作线程是否需要退出，以及告诉调度线程是否需要唤醒线程（注意想要裁剪工作线程必须首先唤醒它），还有需要添加多少线程
     *  
     * 
     */
    class ThreadsPool{
        TESTING_MESSAGE
    private:
        /// @brief 原子变量，标记线程池是否需要关闭
        std::atomic<bool> is_shutdown;
        int max_thread_num;           ///< 最大线程数，只由调度线程维护和访问，无条件竞争
        int min_thread_num;           ///< 最小线程数（核心线程数），无条件竞争        
            // 有条件竞争的对象, manager 访问时需要进行快照
        /**
         * @brief 存活线程数
         * @details 
         *      需要被工作线程（调用 is_needed_exit()）或调度线程访问和修改，存在条件竞争
         * 
         */
        std::atomic<int> live_thread_num;                // 存活线程数（等待任务的线程数 + 正在工作的线程数）  
                                            // 可由 manager_thread 和 thr_pool[i] 修改
        /**
         * @brief 繁忙线程数
         * @details
         *      需要被工作线程（调用 is_needed_exit()）或调度线程访问和修改，存在条件竞争
         */
        std::atomic<int> busy_thread_num;
        /// @brief 用于保护线程池的锁，搭配条件变量使用
        std::mutex mtx_thr_pool;
        /// @brief 条件变量，用于唤醒线程
        std::condition_variable cv_thr_pool;
        /// @brief std::vector<> 实现的线程池，存储创建的线程对象
        std::vector<std::thread> thr_pool;   
        /// @brief 调度线程，在线程池开启时就会启动
        std::thread manager_thread;         
        /// @brief 线程安全的队列，用于存储待执行的任务
        AntonaStandard::Utilities::ThreadSafeQueue<std::function<void()>> tasks;    // 存储任务的队列
    private:
        /**
         * @brief 由工作线程调用的工作函数
         * @details
         *      该函数负责从任务队列中取出任务并且执行。另外还负责检查自己是否需要退出（销毁）
         * 
         */
        void work();
        /**
         * @brief 由调度线程调用的调度函数
         * @details
         *      该函数通过定期轮询的方式管理工作线程，维持工作线程总数大于等于核心线程数以及小于最大线程数
         *      同时还负责接收和处理用户关闭线程池的请求。
         */
        void manage();
    protected:
        /**
         * @brief 获取调度线程轮询的时间间隔。
         * @details
         *      如果轮询不设置间隔时间会导致CPU的占用上升，浪费系统资源
         * 
         * @return std::chrono::milliseconds 
         */
        virtual std::chrono::milliseconds get_manager_waiting_period();
        /**
         * @brief 由调度线程调用，告诉调度线程当前状态下需要新建多少线程
         * @details
         *      这个状态是某一时刻的快照（某一时刻线程池任务量，工作线程数）而不是实时的。因为为想在不添加过多的性能损耗的前提下
         *      保证其原子性。这个快照一般是调度线程获取到线程池的锁时统计的，不会有条件竞争的问题
         * @param live_thread_num_snapshot 
         * @param busy_thread_num_snapshot 
         * @param tasks_num_snap 
         * @return int 
         */
        virtual int get_add_thread_num( int live_thread_num_snapshot, 
                                        int busy_thread_num_snapshot,
                                        int tasks_num_snap);
        /**
         * @brief 由调度线程调用，告诉调度线程当前状态下是否需要唤醒线程完成任务
         * 
         * @param live_thread_num_snapshot 
         * @param busy_thread_num_snapshot 
         * @param tasks_num_snapshot 
         * @return true 
         * @return false 
         */
        virtual bool is_need_motivate(  int live_thread_num_snapshot, 
                                        int busy_thread_num_snapshot,
                                        int tasks_num_snapshot);
        /**
         * @brief 由工作线程调用，告诉工作线程自己是否应该退出（销毁）
         * @details
         *      在工作线程调用任务函数之前会先调用该函数判断自己是否退出，如果是则会修改 live_thread_num 的值
         *      否则会修改busy_thread_num 的值然后调用任务
         * 
         * @param live_thread_num_snapshot 
         * @param busy_thread_num_snapshot 
         * @param tasks_num_snapshot 
         * @return true 
         * @return false 
         */
        virtual bool is_needed_exit( int live_thread_num_snapshot,
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
        /**
         * @brief 由用户调用，用于启动线程池
         * 
         */
        void lauch();
        /**
         * @brief 由用户调用，用于关闭线程池
         * 
         */
        void shutdown();
        /**
         * @brief 由用户调用，用于向线程池提交任务
         * @details
         *      本项目的线程池支持提交任意类型的函数或仿函数作为任务（对于类成员函数可以用std::function 和 std::bind 包装一下）
         * @tparam type_Func 
         * @tparam type_Args 
         * @param func 
         * @param args 
         * @return std::future<decltype(func(args...))> 这个返回值可以异步获取任务的返回值
         */
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
