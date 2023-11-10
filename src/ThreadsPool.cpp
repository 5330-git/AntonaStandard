#include "ThreadTools/ThreadsPool.h"

#include <iostream>
namespace AntonaStandard::ThreadTools {
    void ThreadsPool::work(){
        std::function<void(void)> work_task;
        while(!this->is_shutdown.load(std::memory_order_relaxed)){
            {
                std::unique_lock<std::mutex> lck(this->mtx_thr_pool);
                while(this->tasks.empty() && !this->is_shutdown.load(std::memory_order_relaxed)){
                    // 生成快照
                    int live_thread_num_snapshot = this->live_thread_num.load(std::memory_order_relaxed);
                    int busy_thread_num_snapshot = this->busy_thread_num.load(std::memory_order_relaxed);
                    int tasks_num_snapshot = this->tasks.size();
                    // 判断是等待还是退出
                    if(this->is_neeed_exit(live_thread_num_snapshot, busy_thread_num_snapshot, tasks_num_snapshot)){
                        this->live_thread_num.fetch_sub(1, std::memory_order_release);
                        return;
                    }
                    else{
                        this->cv_thr_pool.wait(lck);
                    }

                }
            }
            if(this->tasks.pop(work_task)){
                // 取到任务
                this->busy_thread_num.fetch_add(1,std::memory_order_release);
                work_task();
                this->busy_thread_num.fetch_sub(1,std::memory_order_release);
            }
        }
    }
    void ThreadsPool::manage(){
        // 管理线程
        // 休眠等待
        while(!this->is_shutdown.load(std::memory_order_relaxed)){
            std::this_thread::sleep_for(this->get_manager_waiting_period());
            // 生成快照，要求总是读取到最新的数据（读取需要排在在写入之后）
            int live_thread_num_snap = this->live_thread_num.load(std::memory_order_relaxed);
            int busy_thread_num_snap = this->busy_thread_num.load(std::memory_order_relaxed);
            int task_num_snap = this->tasks.size();

            // 计算需要增加的线程数
            int add_thread_num = this->get_add_thread_num(
                                    live_thread_num_snap,
                                    busy_thread_num_snap,
                                    task_num_snap);
            // 遍历线程池中的线程
            if(add_thread_num > 0){
                for(int i = 0; add_thread_num > 0 && i < this->thr_pool.size(); i++){
                    // 检查线程的存活情况
                    if(this->thr_pool[i].get_id() == std::thread::id()){
                        // 线程死亡，重新创建
                        this->thr_pool[i] = std::move(std::thread(&ThreadsPool::work,this));
                        --add_thread_num;
                        this->live_thread_num.fetch_add(1,std::memory_order_release);
                    }
                }
            }
            // 重新采集快照,要求总是读取到最新的数据（读取需要排在在写入之后）
            live_thread_num_snap = this->live_thread_num.load(std::memory_order_relaxed);
            busy_thread_num_snap = this->busy_thread_num.load(std::memory_order_relaxed);
            task_num_snap = this->tasks.size();

            
            // 计算是否需要唤醒所有线程处理任务，或者需要唤醒多余的线程自行缩减
            bool if_motivate = this->is_need_motivate(
                                    live_thread_num_snap,
                                    busy_thread_num_snap,
                                    task_num_snap);
                                    
            if(if_motivate){
                this->cv_thr_pool.notify_all();
            }
        }
    }

    std::chrono::milliseconds ThreadsPool::get_manager_waiting_period(){
        return std::chrono::milliseconds(5000);
    }

    int ThreadsPool::get_add_thread_num( int live_thread_num_snapshot, 
                                    int busy_thread_num_snapshot,
                                    int tasks_num_snap){
        // 实现计算需要增加的线程数的算法
        int result = 0;
        if(live_thread_num_snapshot < this->min_thread_num){
            // 小于最少线程数
            result += this->min_thread_num - live_thread_num_snapshot;
        }
        // 繁忙线程数较多，并且任务总数较多
        if(busy_thread_num_snapshot * 2 > live_thread_num_snapshot&& 
            tasks_num_snap > this->max_thread_num*2 &&
            live_thread_num_snapshot + 2 < this->max_thread_num){
            result += 2;
        }
        
        return result;
    }
    bool ThreadsPool::is_need_motivate(  int live_thread_num_snapshot, 
                                    int busy_thread_num_snapshot,
                                    int tasks_num_snapshot){
        // 任务量激增，或者需要裁剪多余的线程，需要通过该函数的算法进行判断
        if(tasks_num_snapshot == 0){
            
            if(this->is_shutdown.load(std::memory_order_relaxed)){
                return true;
            }
            if(live_thread_num_snapshot > this->min_thread_num){
                // 存活线程大于核心线程，需要唤醒进行裁剪
                return true;
            }
            // 没有任务且线程池未关闭不需要唤醒
            return false;
        }
        if(busy_thread_num_snapshot  < live_thread_num_snapshot &&
            tasks_num_snapshot > busy_thread_num_snapshot*1.5){
            // 存在部分工作线程没有工作，并且任务数量较多
            return true;
        }
        return false;
    }
    bool ThreadsPool::is_neeed_exit( int live_thread_num_snapshot,
                                int busy_thread_num_snapshot,
                                int tasks_num_snapshot){
        // 判断工作线程是否应该退出
        if(live_thread_num_snapshot <= min_thread_num){
            // 小于等于最小线程数，不应该退出
            return false;
        }
        if(busy_thread_num_snapshot * 2 > live_thread_num_snapshot){
            // 工作的线程占总线程数的半以上，可用的线程数偏小不应该退出
            return false;
        }
        if(tasks_num_snapshot*2 > live_thread_num){
            // 待取任务较多
            return false;
        }
        return true;
    }
    ThreadsPool:: ~ThreadsPool(){
        if(!this->is_shutdown){
            this->shutdown();
        }
        // 任务队列随后会销毁，同时内部的future对象会被销毁，结果获取处会接收到 std::future_error
    }
    void ThreadsPool::lauch(){
        // 启动线程池（创建最小线程数个线程）
        for(int i = 0; i < this->min_thread_num; ++i){
            this->thr_pool[i] = std::thread(&ThreadsPool::work, this);
        }
        // 启动manager
        this->manager_thread = std::thread(&ThreadsPool::manage, this);
    }
    void ThreadsPool::shutdown(){
        // 关闭线程池
        this->is_shutdown.store(true,std::memory_order_release);       // 优先修改
        // 关闭管理者线程（防止它创建新线程）
        if(this->manager_thread.joinable()){
            this->manager_thread.join();
        }
        
        // 关闭剩余线程
        for(auto& thr:this->thr_pool){
            if(thr.joinable()){
                thr.join();
            }
        }
        this->tasks.clear();
        
    }
}