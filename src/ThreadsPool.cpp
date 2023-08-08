#include "ThreadTools/ThreadsPool.h"

#include <iostream>
namespace AntonaStandard::ThreadTools {
    void ThreadsPool::work() {
        while(true){
            // 等待超过20秒自动销毁
            std::unique_lock<std::mutex> lck_pool(this->protect_pool_mtx);
            // 加判断，防止强制结束和队列不空的唤醒丢失。其中强制结束的唤醒非常重要，否则会造成死锁
            while(!this->stop && this->tasks.empty()){
                this->task_queue_cv.wait_for(lck_pool,std::chrono::seconds(this->max_seconds));
                if(!this->stop && this->tasks.empty()){          // 仍然不满足唤醒条件，说明是超时唤醒
                    // 等待dead_threads的使用权
                    dead_threads.insert(std::pair<std::thread::id,std::thread*>(std::this_thread::get_id(),this->threads[std::this_thread::get_id()]));
                    // this->threads[std::this_thread::get_id()] = nullptr;
                    this->threads.erase(std::this_thread::get_id());
                    // 容器中的指针的擦除工作由管理者线程来执行
                    return ;            // 到时仍未等到任务或者线程池要求终止时，自动结束
                }
            }
            
            
            if(this->stop && this->tasks.empty()){
                                    // 必须队列为空了才能彻底停止
                return;
            }
            // 取任务
            auto task = this->tasks.front();
            this->tasks.pop();
            // 执行任务
            task();
        }
    }

    void ThreadsPool::stratagy() {
        // 管理者管理策略
        while(true){
            
            {
                // 锁定线程池
                std::unique_lock<std::mutex> lck_pool(this->protect_pool_mtx);
                int cur_threads_counts = this->threads.size();
                int cur_tasks_counts = this->tasks.size();
                int added_threads = 0;
                // 维持核心线程数
                if(!this->stop && cur_tasks_counts < this->coreCount){
                    // 小于核心线程数,则添加合适的个数个线程
                    for(int i = 0;i<this->coreCount-cur_tasks_counts;++i){
                        std::thread* th_ptr = new std::thread(&ThreadsPool::work,this);
                        threads.insert(std::pair<std::thread::id,std::thread*>(th_ptr->get_id(),th_ptr));
                        ++added_threads;
                    }
                    
                }
                // 平衡任务数量和线程数量
                if(!this->stop && cur_tasks_counts> (cur_tasks_counts+added_threads)*2){
                    // 添加一个线程
                    std::thread* th_ptr = new std::thread(&ThreadsPool::work,this);
                    threads.insert(std::pair<std::thread::id,std::thread*>(th_ptr->get_id(),th_ptr));
                }
                // 清除死亡线程
                for(auto& t:this->dead_threads){
                    // 从threads中擦除                    
                    t.second->detach();         // 分离线程
                    delete t.second;
                    t.second = nullptr;
                    
                }
                this->dead_threads.clear();
                if(this->stop && this->tasks.empty()){
                    return;
                }
            }
            
            
            
            // 首先休眠5秒
            std::this_thread::sleep_for(std::chrono::seconds(5));
        }
    }

    ThreadsPool::ThreadsPool(int coreC):stop(false),coreCount(coreC){
        // 核心线程数默认为0
        for(int i = 0;i<coreCount;++i){
            std::thread* th_ptr = new std::thread(&ThreadsPool::work,this);
            threads.insert(std::pair<std::thread::id,std::thread*>(th_ptr->get_id(),th_ptr));
        }
        // 创建管理者
        this->manager_ptr = new std::thread(&ThreadsPool::stratagy,this);
    }  

    ThreadsPool::~ThreadsPool() {
        {
            std::unique_lock<std::mutex> lck_pool(this->protect_pool_mtx);
            this->stop = true;  // 修改标记，终止所有线程
        }
        this->task_queue_cv.notify_all();
        // 等待管理者线程结束，管理者线程结束时所有工作线程都处于循环开始的地方，此时stop条件已经为真，因此不会再有线程因为超时而退出
        this->manager_ptr->join();
        delete this->manager_ptr;
        // 不会再有线程进行超时终结，即threads和deadthreads不会再被修改
        // 遍历活着的线程
        for (auto& t : this->threads) {
            if (t.second) {
                // 判断的过程是防止，t对应的线程在遍历过程中被清除
                t.second->join();
                delete t.second;
            }
        }
    }
}