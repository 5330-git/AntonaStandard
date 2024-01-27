#ifndef THREADTOOLS_SEM_EXTENSION_H
#define THREADTOOLS_SEM_EXTENSION_H
#include <semaphore>
#include <mutex>
#include "TestingSupport/TestingMessageMacro.h"
#define AntonaStandard_ThreadTools_Sem_Extension_VERSION "2.1.0"
#define AntonaStandard_ThreadTools_Sem_Extension_EDIT_TIME "2023/8/8"
#define AntonaStandard_ThreadTools_Sem_Extension_AUTHOR "Anton"

/*
*   Decoded by UTF-8
*   2023/4/11 v-1.0.0 初步实现：and信号量请求，信号量集请求
*   2023/7/9  v-2.0.0 由于原来的模板函数声明在Linux下无法识别非模板参数sem_max_counts,本版本将信号量的类型也用模板参数抽象了出来，因此模板函数声明有较大改变
*   2023/8/8  v-2.1.0 修改命名空间从 AntonaStandard 到 AntonaStandard::ThreadTools
*/
// And信号量请求
namespace AntonaStandard{
    namespace ThreadTools{
        class And_Sem_Acquirer;                 // And信号量请求者，每个请求者维护一种同步问题
        class Sem_Set_Acquirer;    
    }            
}
namespace AntonaStandard::ThreadTools{
    class And_Sem_Acquirer{
        TESTING_MESSAGE
    private:
        std::mutex and_sem_mutex;
        // 检查传入的若干信号量是否可以被同时锁定，如果可以就返回ture,否则返回false
        template<typename type_Sem,typename... type_Args>
        bool and_sem_isAvaliable(
            type_Sem&& sem,
            type_Args&&... args
        );

        // 模板函数 and_sem_isAvaliable的参数展开递归出口
        template<typename type_Sem>
        bool and_sem_isAvaliable(
            type_Sem& sem
        );

        // 用于包装 and_isAvaliable 使得该函数可以互斥执行
        template<typename type_Sem,typename... type_Args>
        bool and_sem_testing(
            type_Sem&& sem,
            type_Args&&... args
        );

    public:
        And_Sem_Acquirer(){};
        // 用于以And信号量的方式发出占用请求
        template<typename type_Sem,typename... type_Args>
        void and_acquire(
            type_Sem&& sem,
            type_Args&&... args
        );
        // 释放信号量
        template<typename type_Sem,typename... type_Args>
        void and_release(
            type_Sem&& sem,
            type_Args&&... args
        );
        // 释放信号量的参数展开递归出口
        template<typename type_Sem>
        void and_release(
            type_Sem& sem
        );
    };

    class Sem_Set_Acquirer{
        TESTING_MESSAGE
    private:
        std::mutex sem_set_mutex;
        // 请求 n 个资源
        template<typename type_Sem>
        bool try_acquire_n(
            type_Sem& sem,
            int n
        );

        // 可变模板函数递归调用的出口
        template<typename type_Sem>
        bool sem_set_isAvaliable(
            type_Sem& sem,
            int ava_least,
            int ava_counts
        );

        // 可变模板函数，检查传入的若干信号量是否可用
        template<typename type_Sem,typename... type_Args>
        bool sem_set_isAvaliable(
            type_Sem&& sem,
            int ava_least,
            int ava_counts,
            type_Args&&... args
        );
    
        // 包装sem_set_isAvaliavle 是其递归调用是互斥的
        template<typename type_Sem,typename... type_Args>
        bool sem_set_testing(
            type_Sem&& sem,
            int ava_least,
            int ava_counts,
            type_Args&&... args
        );
    
        

    public:
        Sem_Set_Acquirer(){};
        // 信号量请求
        template<typename type_Sem,typename... type_Args>
        void sem_set_acquire(
            type_Sem&& sem,
            int ava_least,
            int ava_counts,
            type_Args&&... args
        );

        // 信号量释放的递归出口
        template<typename type_Sem>
        void sem_set_release(
            type_Sem&& sem,
            int ava_counts
        );

        // 信号量释放
        template<typename type_Sem,typename... type_Args>
        void sem_set_release(
            type_Sem&& sem,
            int ava_counts,
            type_Args&&... args
        );
    };

    

}

namespace AntonaStandard::ThreadTools{
    template <typename type_Sem,typename... type_Args>
    inline bool And_Sem_Acquirer::and_sem_isAvaliable(type_Sem&& sem, type_Args &&...args)
    {
        // 首先尝试锁定sem,如果锁定成功就接着锁剩下的
        if(sem.try_acquire()){
            // 成功
            if(and_sem_isAvaliable(args...)){
                // 锁定成功
                return true;
            }
            // 锁定失败，将sem解锁
            sem.release();
        }
        // 失败
        return false;
    }

    template <typename type_Sem>
    inline bool And_Sem_Acquirer::and_sem_isAvaliable(type_Sem &sem)
    {
        return sem.try_acquire();
    }

    template<typename type_Sem,typename... type_Args>
    inline bool And_Sem_Acquirer::and_sem_testing(type_Sem&& sem, type_Args &&...args)
    {
        // 使用互斥锁and_sem_mutex,保证对and_sem_isAvaliable的访问是互斥的
        typename std::unique_lock<std::mutex> lck(this->and_sem_mutex);
        return this->and_sem_isAvaliable(sem,args...);
    }

    template<typename type_Sem,typename... type_Args>
    inline void And_Sem_Acquirer::and_acquire(type_Sem&& sem, type_Args &&...args)
    {
        while(!this->and_sem_testing(sem,args...)){
            // 如果没有办法一次性分配，那就让出时间片
            std::this_thread::yield();
        }
    }

    template<typename type_Sem,typename... type_Args>
    inline void And_Sem_Acquirer::and_release(type_Sem&& sem, type_Args &&...args)
    {
        this->and_release(args...);
        sem.release();
    }

    template <typename type_Sem>
    inline void And_Sem_Acquirer::and_release(type_Sem &sem)
    {
        sem.release();
    }

    template <typename type_Sem>
    inline bool Sem_Set_Acquirer::try_acquire_n(type_Sem&sem, int n)
    {
        for(int i = 1;i<=n;++i){
            if(!sem.try_acquire()){
                // 第i次请求失败，释放掉前i-1次申请的信号量
                sem.release(i-1);
                return false;
            }
        }
        return true;
    }

    template <typename type_Sem>
    inline bool Sem_Set_Acquirer::sem_set_isAvaliable(type_Sem &sem, int ava_least, int ava_counts)
    {
        // 查看是否满足下限，即可申请的信号量大于 ava_least
        if(try_acquire_n(sem,ava_least)){
            // 满足下线，查看是否可以一次性申请ava_counts个资源
            sem.release(ava_least);
            if(try_acquire_n(sem,ava_counts)){
                // 可以一次性申请，那么返回ture即可
                return true;
            }
        }
        return false;
    }

    template<typename type_Sem,typename... type_Args>
    inline bool Sem_Set_Acquirer::sem_set_isAvaliable(type_Sem&& sem, int ava_least, int ava_counts, type_Args &&...args)
    {
        // 查看是否满足下限，即可申请的信号量大于 ava_least
        if(try_acquire_n(sem,ava_least)){
            // 满足下线，查看是否可以一次性申请ava_counts个资源
            sem.release(ava_least);
            if(try_acquire_n(sem,ava_counts)){
                // 可以一次性申请，递归考察剩下的信号量是否可以申请成功
                if(sem_set_isAvaliable(args...)){
                    // 都可以锁定，返回true
                    return true;
                }
                // 不可以锁定，释放掉信号量
                sem.release(ava_counts);
            }
        }
        return false;
    }

    template<typename type_Sem,typename... type_Args>
    inline bool Sem_Set_Acquirer::sem_set_testing(type_Sem&& sem, int ava_least, int ava_counts, type_Args &&...args)
    {
        typename std::unique_lock<std::mutex> lck(this->sem_set_mutex);
        return sem_set_isAvaliable(sem,ava_least,ava_counts,args...);
    }

    template <typename type_Sem>
    inline void Sem_Set_Acquirer::sem_set_release(type_Sem&& sem, int ava_counts)
    {
        sem.release(ava_counts);
    }

    template<typename type_Sem,typename... type_Args>
    inline void Sem_Set_Acquirer::sem_set_acquire(type_Sem&& sem, int ava_least, int ava_counts, type_Args &&...args)
    {
        while(!sem_set_testing(sem,ava_least,ava_counts,args...)){
            // 申请不成功就让出时间片
            std::this_thread::yield();
        }
    }
    
    template<typename type_Sem,typename... type_Args>
    inline void Sem_Set_Acquirer::sem_set_release(type_Sem&& sem, int ava_counts, type_Args &&...args)
    {
        sem_set_release(args...);
        sem.release(ava_counts);
    }
}
#endif