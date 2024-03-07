/**
 * @file Delegate.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 实现简单的事件委托
 * @details
 *      模拟实现了 std::function 和 std::bind 的功能
 *      该项目更多是用于练手，研究模板编程，因为论功能和安全性都比不过 std::function 和 std::bind（std::function 可以通过异步框架保证线程安全）
 * @warning
 *      该事件委托线程不安全，请在必要的时刻加锁
 * @version 0.1
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef UTILITIES_DELEGATE_H
#define UTILITIES_DELEGATE_H
#include <typeinfo>
#include <vector>
#include <list>
#include <stdexcept>
#include <Globals/Exception.h>
#include <TestingSupport/TestingMessageMacro.h>


// 前置声明：forward declaration
namespace AntonaStandard{
    namespace Utilities{
            // 抽象基类，为不同类型指针的存储提供多态接口
        template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
        class BaseFuncPointerContainer;

            // 存储类非静态成员函数的函数指针的容器，由BaseFuncPointerContainer派生
        template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
        class MethodFuncPointerContainer;

            // 存储普通函数和静态成员函数指针的容器(普通函数和静态成员函数的指针属性相同)，由BaseFuncPointerContainer派生 
        template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
        class Static_NormalFuncPointerContainer;

        /**
         * @brief 委托类的主模板，包含两个特化模板类
         * 
         * @tparam type_RETURN_VALUE 
         * @tparam type_PARAMETERS_PACK 
         */
        template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
        class Delegate;


            // 为指针容器创建提供统一接口
                // 返回普通函数或静态成员函数的重载版本
        template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
        Static_NormalFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>*
        newDelegate( type_RETURN_VALUE(*func_ptr)(type_PARAMETERS_PACK...) );      // 注意指针类型的声明

                // 返回非静态成员函数容器的重载版本
        template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
        MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>*
        newDelegate(type_OBJECT* obj_ptr, type_RETURN_VALUE(type_OBJECT::*func_ptr)(type_PARAMETERS_PACK...)); 

        template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
        MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>*
        newDelegate(type_OBJECT& obj_ptr, type_RETURN_VALUE(type_OBJECT::*func_ptr)(type_PARAMETERS_PACK...)); 
    }
}

// 模板类声明： declaration of template class

namespace AntonaStandard::Utilities{
    /**
     * @brief 函数指针容器基类，将在其基础上派生出静态函数/普通函数 以及成员函数的容器类
     * 
     * @tparam type_RETURN_VALUE 
     * @tparam type_PARAMETERS_PACK 
     */
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    class BaseFuncPointerContainer{
        TESTING_MESSAGE
    public:
        BaseFuncPointerContainer(){};
            // 声明虚析构函数
        virtual ~BaseFuncPointerContainer(){};
        /**
         * @brief 检查当前对象的类型和参数指定的类型是否相同
         * 
         * @param type 
         * @return true 
         * @return false 
         */
        virtual bool isType(const std::type_info& type)=0;
        /**
         * @brief 调用保存的函数指针的接口
         * 
         * @param para_args 
         * @return type_RETURN_VALUE 
         */
        virtual type_RETURN_VALUE call(type_PARAMETERS_PACK... para_args)=0;
        /**
         * @brief 判断两个容器中的函数指针是否相等
         * 
         * @param container_ptr 
         * @return true 
         * @return false 
         */
        virtual bool equal(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* container_ptr)const=0;
        /**
         * @brief 对容器进行浅拷贝
         * 
         * @return BaseFuncPointerContainer* 
         */
        inline virtual BaseFuncPointerContainer* clone()=0;
        // inline virtual BaseFuncPointerContainer* copy(const BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>& other)=0;
        /**
         * @brief 对容器进行深拷贝
         * 
         * @return BaseFuncPointerContainer* 
         */
        inline virtual BaseFuncPointerContainer* copy()=0;
    };
    // 保存非静态成员函数指针的容器
    /**
     * @brief 成员函数包装
     * 
     * @tparam type_OBJECT 
     * @tparam type_RETURN_VALUE 
     * @tparam type_PARAMETERS_PACK 
     */
    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
    class MethodFuncPointerContainer:public BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>{
    public:
            // 类型重命名（函数指针类型）
        using FuncPtr_Type = type_RETURN_VALUE(type_OBJECT::*)(type_PARAMETERS_PACK...);
    protected:
        type_OBJECT* obj_ptr;           // 对象指针
        FuncPtr_Type func_ptr;             // 非静态成员函数指针
    public: 
            // 通过引用构造
        MethodFuncPointerContainer(type_OBJECT& obj,FuncPtr_Type _func_ptr ):obj_ptr(&obj),func_ptr(_func_ptr){};
            // 通过指针构造
        MethodFuncPointerContainer(type_OBJECT* _obj_ptr,FuncPtr_Type _func_ptr):obj_ptr(_obj_ptr),func_ptr(_func_ptr){};

            // 声明虚析构函数
        virtual ~MethodFuncPointerContainer(){};
            // 重写，判断传入的参数是否属于类型
        virtual bool isType(const std::type_info& type)override;
            // 重写，调用函数指针
        virtual type_RETURN_VALUE call(type_PARAMETERS_PACK... para_args)override;
            // 重写，判断两个函数指针及其实例对象是否相等
        virtual bool equal(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* container_ptr)const override;
            // 重写获取样本函数，返回值协变成派生类
        inline virtual MethodFuncPointerContainer* clone()override{
            return this;
        }
            // 重写创建函数，返回值协变（不协变也可以）
        // inline virtual MethodFuncPointerContainer* copy(const BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>& other)override{
        //     const MethodFuncPointerContainer& _other = static_cast<const MethodFuncPointerContainer&>(other);
        //     return new MethodFuncPointerContainer(_other.obj_ptr,_other.func_ptr);
        // }
        inline virtual MethodFuncPointerContainer* copy()override{
            return new MethodFuncPointerContainer(this->obj_ptr,this->func_ptr);
        }
    };

    // 保存静态成员函数和普通函数的容器类
    /**
     * @brief 静态成员函数和普通函数的包装容器
     * 
     * @tparam type_RETURN_VALUE 
     * @tparam type_PARAMETERS_PACK 
     */
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    class Static_NormalFuncPointerContainer:public BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>{
    public:
        using FuncPtr_Type = type_RETURN_VALUE(*)(type_PARAMETERS_PACK...);
    protected:
        FuncPtr_Type func_ptr;
    public:
        
        Static_NormalFuncPointerContainer(FuncPtr_Type _func_ptr ):func_ptr(_func_ptr){};
            // 声明虚析构函数
        virtual ~Static_NormalFuncPointerContainer(){};
            // 重写，判断传入的参数是否属于类型
        virtual bool isType(const std::type_info& type)override;
            // 重写，调用函数指针
        virtual type_RETURN_VALUE call(type_PARAMETERS_PACK... para_args)override;
            // 重写，判断两个函数指针及其实例对象是否相等
        virtual bool equal(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* container_ptr)const override;
            // 重写获取样本函数，返回值协变成派生类
        inline virtual Static_NormalFuncPointerContainer* clone()override{
            return this;
        }
            // 重写创建函数，返回值协变（不协变也可以）
        // inline virtual Static_NormalFuncPointerContainer* copy(const BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>& other)override{
        //     const Static_NormalFuncPointerContainer& _other = static_cast<const Static_NormalFuncPointerContainer&>(other);
        //     return new Static_NormalFuncPointerContainer(_other.func_ptr);
        // }
        inline virtual Static_NormalFuncPointerContainer* copy()override{
            return new Static_NormalFuncPointerContainer(this->func_ptr);
        }
    };  

    /**
     * @brief 委托类
     * @details
     *      通过链表存储函数指针容器
     * 
     * @tparam type_RETURN_VALUE 
     * @tparam type_PARAMETERS_PACK 
     */
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    class Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>{
        TESTING_MESSAGE
        // 通过链表存储容器
    public:
        // 声明一下链表和迭代器，化简表达
        using DelegateList = std::list<BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* >;
        using DelegateListIterator = typename DelegateList::iterator;
        using DelegateListConstIterator = typename DelegateList::const_iterator;
    protected:
        DelegateList delegateList;

    public:
        /// @brief 获取委托数量
        inline virtual unsigned long size()const{
            return this->delegateList.size();
        }
            // 判断委托是否为空

        /// @brief 判断委托调用是否为空
        /// @return 
        inline virtual bool empty()const{
            return this->delegateList.empty();
        }
        /// @brief 清空委托
        virtual void clear();
        /// @brief 添加委托
        /// @param other_ptr 
        /// @return 
        virtual Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>& operator+=(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* other_ptr);
        /// @brief 移除委托
        /// @param other_ptr 
        /// @return 
        virtual Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>& operator-=(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* other_ptr);
            // 多播返回值存储到一个vector中
        /**
         * @brief 调用所有的委托函数
         * @details
         *      所有委托的函数的返回值会脱去引用拷贝到一个vector对象中返回
         * @param parama_args 
         * @return std::vector<typename std::remove_reference<type_RETURN_VALUE>::type> 
         */
        virtual  std::vector<typename std::remove_reference<type_RETURN_VALUE>::type> operator()(type_PARAMETERS_PACK... parama_args);
        /// @brief 返回值如果要存到一个vector中效率较低，这里提供一个无返回值的接口
        virtual void call_without_return(type_PARAMETERS_PACK... parama_args);

        //  左值赋值构造函数，和赋值函数
        Delegate(const Delegate&);
        Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>& operator=(const Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&);
        
        // 右值移动构造函数，和赋值函数
        Delegate(Delegate&&);
        Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>& operator=(const Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&&);
        
        // 空构造函数
        Delegate(){};
        // 虚析构函数
        virtual ~Delegate(){this->clear();};
    };

    /**
     * @brief 委托类，返回值是 void 的特化版本
     * 
     * @tparam type_PARAMETERS_PACK 
     */
    template<typename... type_PARAMETERS_PACK>
    class Delegate<void(type_PARAMETERS_PACK...)>{
        TESTING_MESSAGE
        // 通过链表存储容器
    public:
        // 声明一下链表和迭代器，化简表达
        using DelegateList = std::list<BaseFuncPointerContainer<void,type_PARAMETERS_PACK...>* >;
        using DelegateListIterator = typename DelegateList::iterator;
        using DelegateListConstIterator = typename DelegateList::const_iterator;
    protected:
        DelegateList delegateList;

    public:
        // 声明为虚函数，提高可拓展性
            // 获取委托个数
        inline virtual unsigned long size()const{
            return this->delegateList.size();
        }
            // 判断委托是否为空
        inline virtual bool empty()const{
            return this->delegateList.empty();
        }
            // 清空委托
        virtual void clear();
        virtual Delegate<void(type_PARAMETERS_PACK...)>& operator+=(BaseFuncPointerContainer<void,type_PARAMETERS_PACK...>* other_ptr);
        virtual Delegate<void(type_PARAMETERS_PACK...)>& operator-=(BaseFuncPointerContainer<void,type_PARAMETERS_PACK...>* other_ptr);
            // 暂时禁用多播返回值，因为如果type_RETURN_VALVE为一个引用类型的话std::vector无法进行存储
        virtual void operator()(type_PARAMETERS_PACK... parama_args);
        // virtual void operator()(type_PARAMETERS_PACK... parama_args);

        //  左值赋值构造函数，和赋值函数
        Delegate(const Delegate&);
        Delegate<void(type_PARAMETERS_PACK...)>& operator=(const Delegate<void(type_PARAMETERS_PACK...)>&);
        
        // 右值移动构造函数，和赋值函数
        Delegate(Delegate&&);
        Delegate<void(type_PARAMETERS_PACK...)>& operator=(const Delegate<void(type_PARAMETERS_PACK...)>&&);
        
        // 空构造函数
        Delegate(){};
        // 虚析构函数
        virtual ~Delegate(){this->clear();};
    };
}

// 相关函数定义
namespace AntonaStandard::Utilities{
    // 定义存储非静态成员函数的容器类
        // 判断是否是同类型，用typeid进行比较
    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
    bool 
    MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>::
    isType
    (const std::type_info& type){
        return (typeid(MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>)==type);
    }

        // 调用函数指针
    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
    type_RETURN_VALUE 
    MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>::
    call
    (type_PARAMETERS_PACK... para_args){
        return (this->obj_ptr->*(this->func_ptr))(para_args...);
    }
        // 判断实例对象和调用的函数指针是否相等

    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK>
    bool
    MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>::
    equal
    (BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* container_ptr)const {
        if(container_ptr == nullptr){
            return false;
        }if (!container_ptr->isType(typeid(MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>))){
            // 判断container_ptr的类型是否与当前对象的类型相等
            return false;
        }
        // 比较实例对象和函数指针的地址是否相同
        MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>* cast = 
        static_cast<MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>*>(container_ptr);
        return this->obj_ptr == cast->obj_ptr && this->func_ptr == cast->func_ptr;
        
    }

    // 静态成员函数指针和普通函数指针容器类的成员定义
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    bool
    Static_NormalFuncPointerContainer<type_RETURN_VALUE, type_PARAMETERS_PACK...>::
    isType
    (const std::type_info &type){
        return (typeid(Static_NormalFuncPointerContainer<type_RETURN_VALUE, type_PARAMETERS_PACK...>) == type);
    }

    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    type_RETURN_VALUE
    Static_NormalFuncPointerContainer<type_RETURN_VALUE, type_PARAMETERS_PACK...>::
    call(type_PARAMETERS_PACK... para_args){
        return (*(this->func_ptr))(para_args...);
    }

    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    bool
    Static_NormalFuncPointerContainer<type_RETURN_VALUE, type_PARAMETERS_PACK...>::
    equal(BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* container_ptr)const{
        if(container_ptr == nullptr){
            return false;
        }if (!container_ptr->isType(typeid(Static_NormalFuncPointerContainer<type_RETURN_VALUE, type_PARAMETERS_PACK...>))){
            // 判断container_ptr的类型是否与当前对象的类型相等
            return false;
        }
        // 比较函数指针的地址是否相同
        Static_NormalFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* cast = 
        static_cast<Static_NormalFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>*>(container_ptr);
        return this->func_ptr == cast->func_ptr;
        
    }

    // 委托类
        // 清理委托
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    void 
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    clear(){
        // 遍历链表，删除其中存储的容器指针
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            delete *iter;       // 删除iter指向的内容（注意不是删除iter）
            ++iter;
        }
        // 调用链表的清除函数
        this->delegateList.clear();
    }

        // += 运算符重载
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    operator+=
    (BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* other_ptr){
        // 接受容器指针（抽象基类指针，多态）,遍历列表查看是否有重复的函数，有重复的就停止遍历否则插入在末尾
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            // 不同于参考代码（具体可以看我的博客）在删除委托（-=）是只是将链表结点中的指针清除了，我们这里通过erase同时清除了对应的结点，
            // 因此保证每一个节点保存的容器指针一定不为空
            // 清除了对应迭代器虽然会使得被清除的迭代器失效，但是所有的增删操作都封装在类中，不会出现再次使用被删除的迭代器的情况
            if((*iter)->equal(other_ptr)){
                // other_ptr是分配了内存的堆对象，需要删除
                delete other_ptr;
                other_ptr = nullptr;            // 标记成nullptr，方便debug
                return *this;
            }
            ++iter;
        }
        // 循环正常结束，说明没有重复的函数指针，因此将other_ptr插入到链表的末尾
        this->delegateList.push_back(other_ptr);
        return *this;
    }

        // -= 运算符重载
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    operator-=
    (BaseFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>* other_ptr){
        // 依次比较函数指针，如果不存在抛出异常AntonaStandard::NotFound_Error
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            if((*iter)->equal(other_ptr)){
                // 找到链表中与other_ptr保存的函数指针相等的容器，删除other_ptr和iter
                delete other_ptr;
                other_ptr = nullptr;
                delete (*iter);
                this->delegateList.erase(iter);
                // 删除iter !
                
                return *this;
            }
            ++iter;
        }
        // 删除other_ptr防止内存泄漏
        delete other_ptr;
        other_ptr = nullptr;
        // 循环正常结束，说明没有重复的函数指针,要删除的函数指针未找到，抛出异常，由客户端进行解决
        throw AntonaStandard::Globals::NotFound_Error("The function pointer was not found,fail to delete its container!");
        
    }

        // ()运算符，委托调用存储的函数
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    std::vector<typename std::remove_reference<type_RETURN_VALUE>::type>
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    operator()
    (type_PARAMETERS_PACK ...parama_args){
        std::vector<typename std::remove_reference<type_RETURN_VALUE>::type> temp_vec;
        DelegateListIterator iter = this->delegateList.begin();

        while(iter != this->delegateList.end()){
            temp_vec.push_back((*iter)->call(parama_args...));
            ++iter;
        }
        return std::move(temp_vec);         // 以右值引用的方式构造，避免额外的内存分配
    }

        // 通用模板下事件委托返回值调用
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    void
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    call_without_return(type_PARAMETERS_PACK... parama_args){
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            (*iter)->call(parama_args...);
            ++iter;
        }
    }

    // 左值引用复制构造函数
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    Delegate
    (const Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)> &other){
        // 注意，链表中存储的是函数指针的容器，所以需要我们遍历other分配内存
        DelegateListConstIterator iter = other.delegateList.begin();
        DelegateListConstIterator end = other.delegateList.end();
        while(iter != end){
                // 由原型模式的拷贝函数创建对象（这样可以不依赖于具体的类型名称）
            this->delegateList.push_back((*iter)->copy());
            ++iter;
        }
    }
    // 左值引用赋值运算符
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)> &
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    operator=
    (const Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)> &other){
        // 注意，链表中存储的是函数指针的容器，所以需要我们遍历other分配内存
        DelegateListConstIterator iter = other.delegateList.begin();
        DelegateListConstIterator end = other.delegateList.end();
        while(iter != end){
                // 由原型模式的拷贝函数创建对象（这样可以不依赖于具体的类型名称）
            this->delegateList.push_back((*iter)->copy());
            ++iter;
        }
        return *this;
    }
    // 右值引用移动构造函数
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK> 
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    Delegate
    (Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&& other){
        // std::list有写好的右值赋值运算符，因此不需要我们做额外的操作
        this->delegateList = std::move(other.delegateList);
    }
    // 右值引用赋值运算符
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&
    Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>::
    operator=
    (const Delegate<type_RETURN_VALUE(type_PARAMETERS_PACK...)>&& other){
        this->delegateList = std::move(other.delegateList);
    }

    // void 特化版本成员定义

template<typename... type_PARAMETERS_PACK> 
    void 
    Delegate<void(type_PARAMETERS_PACK...)>::
    clear(){
        // 遍历链表，删除其中存储的容器指针
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            delete *iter;       // 删除iter指向的内容（注意不是删除iter）
            ++iter;
        }
        // 调用链表的清除函数
        this->delegateList.clear();
    }

        // += 运算符重载
    template<typename... type_PARAMETERS_PACK> 
    Delegate<void(type_PARAMETERS_PACK...)>&
    Delegate<void(type_PARAMETERS_PACK...)>::
    operator+=
    (BaseFuncPointerContainer<void,type_PARAMETERS_PACK...>* other_ptr){
        // 接受容器指针（抽象基类指针，多态）,遍历列表查看是否有重复的函数，有重复的就停止遍历否则插入在末尾
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            // 不同于参考代码（具体可以看我的博客）在删除委托（-=）是只是将链表结点中的指针清除了，我们这里通过erase同时清除了对应的结点，
            // 因此保证每一个节点保存的容器指针一定不为空
            // 清除了对应迭代器虽然会使得被清除的迭代器失效，但是所有的增删操作都封装在类中，不会出现再次使用被删除的迭代器的情况
            if((*iter)->equal(other_ptr)){
                // other_ptr是分配了内存的堆对象，需要删除
                delete other_ptr;
                other_ptr = nullptr;            // 标记成nullptr，方便debug
                return *this;
            }
            ++iter;
        }
        // 循环正常结束，说明没有重复的函数指针，因此将other_ptr插入到链表的末尾
        this->delegateList.push_back(other_ptr);
        return *this;
    }

        // -= 运算符重载
    template<typename... type_PARAMETERS_PACK> 
    Delegate<void(type_PARAMETERS_PACK...)>&
    Delegate<void(type_PARAMETERS_PACK...)>::
    operator-=
    (BaseFuncPointerContainer<void,type_PARAMETERS_PACK...>* other_ptr){
        // 依次比较函数指针，如果不存在抛出异常AntonaStandard::NotFound_Error
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            if((*iter)->equal(other_ptr)){
                // 找到链表中与other_ptr保存的函数指针相等的容器，删除other_ptr和iter
                delete other_ptr;
                other_ptr = nullptr;
                // 删除iter !
                delete (*iter);
                this->delegateList.erase(iter);
                return *this;
            }
            ++iter;
        }
        // 删除other_ptr防止内存泄漏
        delete other_ptr;
        other_ptr = nullptr;
        // 循环正常结束，说明没有重复的函数指针,要删除的函数指针未找到，抛出异常，由客户端进行解决
        throw AntonaStandard::Globals::NotFound_Error("The function pointer was not found,fail to delete its container!");
        
    }

        // ()运算符，委托调用存储的函数
    template<class... type_PARAMETERS_PACK> 
    void
    Delegate<void(type_PARAMETERS_PACK...)>::
    operator()
    (type_PARAMETERS_PACK ...parama_args){
        DelegateListIterator iter = this->delegateList.begin();
        while(iter != this->delegateList.end()){
            (*iter)->call(parama_args...);
            ++iter;
        }
    }

    // 左值引用复制构造函数
    template<typename... type_PARAMETERS_PACK> 
    Delegate<void(type_PARAMETERS_PACK...)>::
    Delegate
    (const Delegate<void(type_PARAMETERS_PACK...)> &other){
        // 注意，链表中存储的是函数指针的容器，所以需要我们遍历other分配内存
        DelegateListConstIterator iter = other.delegateList.begin();
        DelegateListConstIterator end = other.delegateList.end();
        while(iter != end){
                // 由原型模式的拷贝函数创建对象（这样可以不依赖于具体的类型名称）
            this->delegateList.push_back((*iter)->copy());
            ++iter;
        }
    }
    // 左值引用赋值运算符
    template<typename... type_PARAMETERS_PACK>
    Delegate<void(type_PARAMETERS_PACK...)> &
    Delegate<void(type_PARAMETERS_PACK...)>::
    operator=
    (const Delegate<void(type_PARAMETERS_PACK...)> &other){
        // 注意，链表中存储的是函数指针的容器，所以需要我们遍历other分配内存
        DelegateListConstIterator iter = other.delegateList.begin();
        DelegateListConstIterator end = other.delegateList.end();
        while(iter != end){
                // 由原型模式的拷贝函数创建对象（这样可以不依赖于具体的类型名称）
            this->delegateList.push_back((*iter)->copy());
            ++iter;
        }
        return *this;
    }
    // 右值引用移动构造函数
    template<typename... type_PARAMETERS_PACK> 
    Delegate<void(type_PARAMETERS_PACK...)>::
    Delegate
    (Delegate<void(type_PARAMETERS_PACK...)>&& other){
        // std::list有写好的右值赋值运算符，因此不需要我们做额外的操作
        this->delegateList = std::move(other.delegateList);
    }
    // 右值引用赋值运算符
    template<typename... type_PARAMETERS_PACK>
    Delegate<void(type_PARAMETERS_PACK...)>&
    Delegate<void(type_PARAMETERS_PACK...)>::
    operator=
    (const Delegate<void(type_PARAMETERS_PACK...)>&& other){
        this->delegateList = std::move(other.delegateList);
    }
    // 创建函数
        // 为指针容器创建提供统一接口
            // 返回普通函数或静态成员函数的重载版本
    template<typename type_RETURN_VALUE, typename... type_PARAMETERS_PACK>
    Static_NormalFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>*
    newDelegate( type_RETURN_VALUE(*func_ptr)(type_PARAMETERS_PACK...) ){
        return new Static_NormalFuncPointerContainer<type_RETURN_VALUE,type_PARAMETERS_PACK...>(func_ptr);
    }

            // 返回非静态成员函数容器的重载版本(由实例对象指针)
    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
    MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>*
    newDelegate(type_OBJECT* obj_ptr, type_RETURN_VALUE(type_OBJECT::*func_ptr)(type_PARAMETERS_PACK...)){
        return new MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>(obj_ptr,func_ptr);
    }

            // 由实例对象引用
    template<typename type_OBJECT,typename type_RETURN_VALUE,typename... type_PARAMETERS_PACK> 
    MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>*
    newDelegate(type_OBJECT& obj, type_RETURN_VALUE(type_OBJECT::*func_ptr)(type_PARAMETERS_PACK...)){
        return new MethodFuncPointerContainer<type_OBJECT,type_RETURN_VALUE,type_PARAMETERS_PACK...>(obj,func_ptr);
    }
}

#endif
