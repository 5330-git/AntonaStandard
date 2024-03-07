#ifndef THREADTOOLS_SEM_EXTENSION_H
#define THREADTOOLS_SEM_EXTENSION_H
/**
 * @file Sem_Extension.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 定义信号量扩展功能
 * @details
 *      信号量是一种并发同步方式。对信号量的数据访问是线程安全的。不同于互斥锁只能由锁定
 *      它的线程解锁，信号量的值可以由任意线程修改，因此使用起来更加灵活
 * @warning
 *      当前版本下依赖C++ 20 标准提供的信号量 std::binary_semaphore 和 std::counting_semaphore 类
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <semaphore>
#include <mutex>
#include <TestingSupport/TestingMessageMacro.h>

namespace AntonaStandard{
    /**
     * @brief 线程工具组件，提供与线程有关的工具
     * 
     */
    namespace ThreadTools{
        class And_Sem_Acquirer;                 // And信号量请求者，每个请求者维护一种同步问题
        class Sem_Set_Acquirer;    
    }            
}
namespace AntonaStandard::ThreadTools{
    /**
     * @brief And 信号量
     * @details
     *      并发编程时常常会因为请求与保持条件（等待别的资源，但是不释放已经持有的资源）而产生死锁。And 信号量
     *      一定程度上可以解决这一问题。And 信号量会同时考察需要获取的资源（信号量）是否可用，如果同时可用
     *      才会锁定资源，否则不会锁定资源
     * 
     */
    class And_Sem_Acquirer{
        TESTING_MESSAGE
    private:
        /**
         * @brief 互斥锁，用于保证对信号量考察时的原子性
         * 
         */
        std::mutex and_sem_mutex;
        // 检查传入的若干信号量是否可以被同时锁定，如果可以就返回ture,否则返回false
        /**
         * @brief 考察传参的信号量是否可用
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 需要请求的信号量
         * @param args 可变模板参数量参数，接收更多的信号量参数
         * @return true 
         * @return false 
         */
        template<typename type_Sem,typename... type_Args>
        bool and_sem_isAvaliable(
            type_Sem&& sem,
            type_Args&&... args
        );

        // 模板函数 and_sem_isAvaliable的参数展开递归出口
        /**
         * @brief C++11可变参数模板需要使用递归进行展开，这里时递归出口
         * 
         * @tparam type_Sem 
         * @param sem 
         * @return true 
         * @return false 
         */
        template<typename type_Sem>
        bool and_sem_isAvaliable(
            type_Sem& sem
        );

        // 用于包装 and_isAvaliable 使得该函数可以互斥执行
        /**
         * @brief 测试传入的信号量是否可用，线程安全，是对 and_isAvaliable() 函数的包装
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param args 
         * @return true 
         * @return false 
         */
        template<typename type_Sem,typename... type_Args>
        bool and_sem_testing(
            type_Sem&& sem,
            type_Args&&... args
        );

    public:
        And_Sem_Acquirer(){};
        // 用于以And信号量的方式发出占用请求
        /**
         * @brief 用于请求信号量资源
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param args 
         */
        template<typename type_Sem,typename... type_Args>
        void and_acquire(
            type_Sem&& sem,
            type_Args&&... args
        );
        // 释放信号量
        /**
         * @brief 用于释放信号量资源
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param args 
         */
        template<typename type_Sem,typename... type_Args>
        void and_release(
            type_Sem&& sem,
            type_Args&&... args
        );
        // 释放信号量的参数展开递归出口
        /**
         * @brief C++ 11 可变模板参数只能通过递归进行展开，这里是展开出口
         * 
         * @tparam type_Sem 
         * @param sem 
         */
        template<typename type_Sem>
        void and_release(
            type_Sem& sem
        );
    };
    /**
     * @brief 信号量集机制
     * @details
     *      信号量虽然可以同时申请多个资源，但是申请一次只能让信号量减一，而信号量集可以指定需要申请的信号量值
     * 
     */
    class Sem_Set_Acquirer{
        TESTING_MESSAGE
    private:
        /// @brief 保障对信号量测试的原子性
        std::mutex sem_set_mutex;
        // 请求 n 个资源
        /**
         * @brief 用于向同一个信号量申请多个资源
         * 
         * @tparam type_Sem 
         * @param sem 
         * @param n 
         * @return true 
         * @return false 
         */
        template<typename type_Sem>
        bool try_acquire_n(
            type_Sem& sem,
            int n
        );

        // 可变模板函数递归调用的出口
        /**
         * @brief 可变模板参数展开出口
         * 
         * @tparam type_Sem 
         * @param sem 
         * @param ava_least 
         * @param ava_counts 
         * @return true 
         * @return false 
         */
        template<typename type_Sem>
        bool sem_set_isAvaliable(
            type_Sem& sem,
            int ava_least,
            int ava_counts
        );

        // 可变模板函数，检查传入的若干信号量是否可用
        /**
         * @brief 判断传入的信号量是否都可用，非原子的
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param ava_least 
         * @param ava_counts 
         * @param args 
         * @return true 
         * @return false 
         */
        template<typename type_Sem,typename... type_Args>
        bool sem_set_isAvaliable(
            type_Sem&& sem,
            int ava_least,
            int ava_counts,
            type_Args&&... args
        );
    
        // 包装sem_set_isAvaliavle 是其递归调用是互斥的
        /**
         * @brief 对 sem_set_isAvaliable() 函数的封装，保障其调用的原子性
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param ava_least 
         * @param ava_counts 
         * @param args 
         * @return true 
         * @return false 
         */
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
        /**
         * @brief 请求信号量集
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 需要请求的信号量
         * @param ava_least 信号量最少量要求，低于这个值不请求
         * @param ava_counts 需要请求的信号量数量
         * @param args 
         */
        template<typename type_Sem,typename... type_Args>
        void sem_set_acquire(
            type_Sem&& sem,
            int ava_least,
            int ava_counts,
            type_Args&&... args
        );

        // 信号量释放的递归出口
        /**
         * @brief 释放信号量，可变参数包递归解包出口
         * 
         * @tparam type_Sem 
         * @param sem 
         * @param ava_counts 
         */
        template<typename type_Sem>
        void sem_set_release(
            type_Sem&& sem,
            int ava_counts
        );

        // 信号量释放
        /**
         * @brief 释放信号量
         * 
         * @tparam type_Sem 
         * @tparam type_Args 
         * @param sem 
         * @param ava_counts 
         * @param args 
         */
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
