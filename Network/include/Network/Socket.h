#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

/**
 * @file Socket.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief   对套接字的面向对象封装
 * @details
 *      本文件基于面向对象封装了TCP的通信套接字、TCP的监听套接字、以及UDP的通信套接字。向用户提供
 *      更加友好的进行套接字通信的方式
 * @version 1.0.0
 * @date 2024-03-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#include <CPS/Socket.h>
#include <Globals/Exception.h>
#include <mutex>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>

namespace AntonaStandard{
    /**
     * @brief Network 组件提供常用的方便使用的网络编程相关的工具
     * 
     */
    namespace Network{
        class Socket;
        class TCPSocket;
        class TCPListenSocket;
        class UDPSocket;

        class SocketManager;
        class SocketOptionValue;
        class SocketChannel;
        class SocketChannelImp;
        class TCPSocketChannelImp;
        class UDPSocketChannelImp;
    }
}

namespace AntonaStandard::Network{
    /**
     * @brief 用于管理套接字文件描述符
     * @details
     *      使用了 RAII 策略一定程度上保障套接字文件描述符能够被释放。但是由于 AntonaStandard::CPS::SocketLibraryManager::close
     *      可能抛出异常，而一般在析构函数中抛出异常可能导致内存泄漏，所以SocketManager 提供了关闭接口，允许用户处理异常
     *      如果用户没有手动关闭套接字文件描述符，SocketManager 的析构函数会替用户做这个操作，而如果出现了异常，它会调用abort
     *      函数中断程序，防止严重的内存泄露事故发生。另外这里将套接字文件描述符从Socket套接字对象中抽离出来单独管理，使用Pimpl策略进行访问
     *      是为了实现一种单例，这样Socket及其派生类不需要考虑是否要禁用拷贝构造的问题，简化了设计。并且让用户使用起来更加方便
     * 
     */
    class SocketManager{
    private:
        CPS::SocketFd fd;
    public:
        SocketManager(CPS::SocketFd fd=INVALID_SOCKET):fd(fd.load()){};
        SocketManager(const SocketManager&) = delete;
        SocketManager(SocketManager&& );

        inline const CPS::SocketFd& getSocketFd()const{
            return this->fd;
        }
        /**
         * @brief 提供给用户手动关闭套接字的接口，让用户有能力处理潜在的异常
         * 
         */
        void close();
        ~SocketManager();
    };
    /**
     * @brief 套接字类的基类
     * @details
     *      使用了 Pimpl 策略，有关套接字文件描述符的管理委托给了类 AntonaStandard::Network::SocketManager
     *      有关通信的操作委托给了 AntonaStandard::Network::SocketChannelImp。同时还管理了一个套接字地址
     */
    class Socket{
    private:
        /// @brief Pimpl 策略指向文件描述符管理器实例的智能指针
        std::shared_ptr<SocketManager> manager;
        /**
         * @brief SocketChannelImp 原型
         * @details
         *      不同类型的套接字通信的手段是不同的，这里使用智能指针允许存储不同类型的套接字通道，以适应不同通信协议下的
         *      通信细节要求。因为与套接字通信相关的功能是委托给 SocketChannelImp 及其派生类完成的。
         *      为了让用户使用接口时更加便捷，让通信的差异相对于用户来说是透明的。这需要
         *      隐藏具体的派生类细节，本项目使用 SocketChannel 类结合 Pimpl 策略隐藏具体的派生类。用户可以通过
         *      Socket 派生类的 getChannel 接口获取这个包装类以完成通信功能。getChannel 函数会将 prototype 直接安装
         *      或拷贝给 返回的 SocketChannel 对象 
         * @see
         *      virtual SocketChannel Socket::getChannel() 
         */
        std::shared_ptr<SocketChannelImp> prototype;
        /**
         * @brief 套接字地址
         * @details
         *      套接字通信需要用到地址，但是不同类型的套接字的地址作用不同，TCP 监听套接字的作用是用来监听本地的地址的，TCP 和 UDP 通信套接字
         *      地址的作用是指明通信地址。所以这里给变量命名的时候采用了 or
         * @see
         *      TCPListenSocket
         *      TCPSocket
         *      UDPSocket
         */
        CPS::SocketAddress bind_or_remote_addr;
        /**
         * @brief std::call_once 的标志变量
         * @details
         *      在launch 函数中通过 std::call_once 保证只调用一次 launch_stuff 函数
         * @see
         *      void launch_stuff()
         *      void launch()
         */
        std::shared_ptr<std::once_flag> flag;
    protected:
        /**
         * @brief 完成启动套接字的一系列操作，由 launch 函数调用
         * @details
         *      不同类型地测套接字启动流程是不同的，比如 TCPListenSocket 启动流程包含：
         *      创建套接字文件描述符，绑定地址，设置监听等步骤。TCPSocket 启动流程是：
         *      创建套接字文件描述符，与目标地址进行连接。另外这些步骤是通过调用 AntonaStandard::CPS::SocketLibraryManager
         *      的接口实现的，这些接口有可能抛出异常，如果放在构造函数内，有可能导致潜在的内存泄漏。故将其抽离出来。同时
         *      为了保证这些操作只会调用一次，这里进一步对 launch_stuff 函数进行包装，由对外接口 launch 函数通过
         *      std::call_once 保证线程安全地唯一调用一次 launch_stuff 函数
         * @see
         *      void launch()
         * @throw
         *      std::runtime_error
         */
        virtual void launch_stuff() = 0;
        /**
         * @brief 获取 SocketChannelImp 原型
         * @return std::shared_ptr<SocketChannelImp> 
         */
        inline std::shared_ptr<SocketChannelImp> getPrototype(){
            return this->prototype;
        }

        /**
         * @brief 用于检查几个智能指针是否是空指针，如果是则抛出空指针异常
         * @details
         *      Pimpl 策略的一大缺点就是带来了访问空指针的风险
         * @param msg 
         * @throw AntonaStandard::Globals::NullPointer_Error 如果几个共享指针保存的资源指针是空指针会抛出
         */
        void assert_nullptr(std::string msg);
        /**
         * @brief 设置套接字选项
         * @details
         *      该函数是 AntonaStandard::CPS::SocketLibraryManager::setSocketOption 的进一步封装。
         *      Socket的派生类会继续基于这个函数提供具体的选项设置接口
         * @param option 
         * @param value 
         * @throw
         *      std::runtime_error
         */
        virtual void setOption(CPS::SocketOptions option,SocketOptionValue value);
        /**
         * @brief 获取套接字选项的值
         * @details
         *      该函数是 AntonaStandard::CPS::SocketLibraryManager::getSocketOption 的进一步封装。
         *      Socket的派生类会基于这个函数提供具体的选项设置接口
         * @param option 
         * @return SocketOptionValue 
         * @throw
         *      std::runtine_error
         */
        virtual SocketOptionValue getOption(CPS::SocketOptions option);


    public:
        Socket() = delete;

        Socket(const CPS::SocketAddress& addr):bind_or_remote_addr(addr){
            this->flag = std::make_shared<std::once_flag>();
        };

        Socket(CPS::SocketAddress&& addr):bind_or_remote_addr(addr){
            this->flag = std::make_shared<std::once_flag>();
        };

        Socket(const Socket& );
        Socket(Socket&& );

        virtual ~Socket() = default;
        /// @brief 获取套接字管理器的共享指针
        inline std::shared_ptr<SocketManager> getManager(){
            return this->manager;
        };
        /// @brief 安装套接字管理器
        /// @param mng 
        inline void install_manager(std::shared_ptr<SocketManager> mng){
            this->manager = mng;
        }
        /// @brief 安装SocketChannelImp 原型
        inline void install_prototype(std::shared_ptr<SocketChannelImp> proto){
            this->prototype = proto;
        }
        /// @brief 获取call_once 标签
        /// @return 
        inline std::shared_ptr<std::once_flag> getOnceFlag(){
            return this->flag;
        }
        /**
         * @brief 获取SocketChannel 对象，用于通信
         * @details
         *      SocketChannel 类是 SocketChannelImp 及其派生类的包装类，所有的功能都是由 SocketChannelImp 完成的
         *      不同通信协议下通信的要求都是不一样的，TCP 通信时，每一个TCP通信套接字只能处理一个连接上的信息交互
         *      而UDP 通信是无连接的，这样每个UDP通信套接字可以与多个地址进行通信。所以对于TCP 通信套接字来说，SocketChannel
         *      相当于一个单例，通过同一个TCPSocket 获取的所有SocketChannel 对象内部维护的SocketChannelImp 对象都是唯一的
         *      而UDPSocket 获取的SocketChannel 对象维护的SocketChannelImp 对象不是唯一的，这样它就可以处理多个通信
         * 
         * @return SocketChannel 
         * @see
         *      SocketChannelImp
         *      SocketChannel
         */
        virtual SocketChannel getChannel();
        /**
         * @brief 启动套接字
         * @details 
         *      对外接口，内部通过 std::call_once 线程安全地唯一调用 launch_stuff 函数，完成套接字启动工作
         * @throw
         *      std::runtime_error
         */
        void launch();
        /**
         * @brief 手动关闭套接字
         * @throw   参考如下函数
         *          void assert_nullptr(std::string)
         *          void SocketManager::close()
         * 
         */
        inline void close(){
            this->assert_nullptr("Socket::close()");
            this->manager->close();
        }
        /// @brief 获取套接字地址
        /// @return CPS::SocketAddress
        inline CPS::SocketAddress& get_bind_or_remote_addr(){
            return this->bind_or_remote_addr;
        }
        /**
         * @brief 设置地址复用，是对setOption 函数的封装
         * @throw
         *          参考 virtual void setOption(CPS::SocketOptions option,SocketOptionValue value);
         */
        void setReuseAddress(bool);
        /**
         * @brief 查看是否开启地址复用
         * 
         * @return true 
         * @return false 
         * @throw
         *          参考 virtual SocketOptionValue Socket::getOption(CPS::SoketOptions)
         */
        bool isReuseAddress();

    };
    /**
     * @brief TCP 监听套接字
     * @details
     *      指定本地地址和端口，可以监听其它端的连接。并且可以通过 accept 函数获取用于通信的TCP通信套接字。
     */
    class TCPListenSocket:public Socket{
    protected:
        /**
         * @brief 启动监听套接字
         * @details
         *      包含以下内容：
         *      - 调用AntonaStandard::CPS::SocketLibraryManager::socket() 创建套接字文件描述符
         *      - 将套接字文件描述符安装给 SocketManager 实例
         *      - 调用AntonaStandard::CPS::SocketLibraryManager::bind() 绑定到bind_or_remote_addr
         *      - 调用AntonaStandard::CPS::SocketLibraryManager::listen() 开启监听
         */
        virtual void launch_stuff()override;
    public:
        TCPListenSocket() = delete;
        TCPListenSocket(const CPS::SocketAddress& addr):Socket(addr){};
        TCPListenSocket(CPS::SocketAddress&& addr):Socket(addr){};
        TCPListenSocket(const TCPListenSocket& sock):Socket(sock){};
        TCPListenSocket(TCPListenSocket&& sock):Socket(sock){};
        /**
         * @brief 等待，接受客户端的连接。返回用于通信的TCP 套接字 TCPSocket
         * @details
         *      内部调用 AntonaStandard::CPS::SocketLibraryManager::accept()
         * @return TCPSocket 
         */
        TCPSocket accept();
        /**
         * @brief 获取用于 TCP 通信的套接字通道
         * @warning
         *      TCP 监听套接字没有通信需求，调用该函数会抛出异常
         *      
         * @return SocketChannel 
         * @throw 
         *      std::runtime_error
         */
        virtual SocketChannel getChannel()override;
    };
    /**
     * @brief 用于通信的TCP 套接字
     * @details
     *      客户端可以直接指定需要通信的服务端地址，进行构造。服务端通过调用 accept 可以获取表示请求连接的客户端套接字
     *      
     */
    class TCPSocket:public Socket{
    protected:
        /**
         * @brief 启动TCP 通信套接字
         * @details
         *      包含以下内容：
         *      - 调用 AntonaStandard::CPS::SocketLibraryManager::socket() 创建套接字文件描述符
         *      - 将套接字文件描述符安装给 SocketManager 实例
         *      - 调用 AntonaStandard::CPS::SocketLibraryManager::connect() 连接到指定地址
         */
        virtual void launch_stuff()override;
    public:
        TCPSocket() = delete;
        TCPSocket(const CPS::SocketAddress& addr):Socket(addr){};
        TCPSocket(CPS::SocketAddress&& addr):Socket(addr){};
        TCPSocket(const TCPSocket& sock):Socket(sock){};
        TCPSocket(TCPSocket&& sock):Socket(sock){};
    };

    /**
     * @brief
     *      用于通信的UDP套接字
     * @details 
     *      UDP 通信双方是对等的，可以直接使用构造函数构造UDP通信套接字
    */
    class UDPSocket:public Socket{
    protected:
    /**
     * @brief 启动UDP通信套接字
     * @details
     *      包含以下内容：
     *      - 调用 AntonaStandard::CPS::SocketLibraryManager::socket() 创建套接字文件描述符
     *      - 将套接字文件描述符安装给 SocketManager 实例
     *      - 调用 AntonaStandard::CPS::SocketLibraryManager::bind() 绑定到本地地址，这样可以等待
     *        其它端发送数据，也可以接收广播数据。
     */
        virtual void launch_stuff()override;
    public:
        UDPSocket() = delete;
        UDPSocket(const CPS::SocketAddress& addr):Socket(addr){};
        UDPSocket(CPS::SocketAddress&& addr):Socket(addr){};
        UDPSocket(const UDPSocket& sock):Socket(sock){};
        UDPSocket(UDPSocket&& sock):Socket(sock){};
    };
    /**
     * @brief 套接字选项值的封装类
     * @details
     *      虽然目前项目支持的套接字选项只有设置地址复用，对应的值是bool类型，表示开启还是关闭。但是为了后面的扩展
     *      需要对所有类型的套接字选项值进行一个抽象。因此这里对套接字选项值做了一个封装，所有的套接字选项值都是通过
     *      一个void类型的指针保存的，因为任何类型的指针都可以自动转化位 void 类型的指针。该类有一个模板构造函数
     *      可以传入任意类型的值（任何基本类型或支持拷贝赋值的类），自动计算该类型的大小，并保存。类型大小可以用作参数
     *      传给 AntonaStandard::CPS::SocketLibraryManager::setSocketOption()。
     *      AntonaStandard::CPS::SocketLibraryManager::getSocketOption()。
     */
    class SocketOptionValue{
    private:
    /**
     * @brief 存储封装的资源的指针
     */
        std::shared_ptr<void> val;
        socklen_t val_size;
    public:
        inline socklen_t size()const{
            return this->val_size;
        }

        inline const std::shared_ptr<void> value_ptr()const{
            return this->val;
        }
        /**
         * @brief 模板构造函数，以适应各种类型的值
         * 
         * @tparam T 
         * @param val 
         */
        template<typename T>
        SocketOptionValue(T val){
            this->val = std::make_shared<T>(val);
            this->val_size = sizeof(val);
        }

        void setValue(const void* val,socklen_t len);
        SocketOptionValue(const void* val,socklen_t len);
        SocketOptionValue(const SocketOptionValue& val);

    };
    /**
     * @brief 套接字通道类的实现接口
     * @details
     *      提供对读写缓冲区的的维护，以及通过对 SocketLibraryManager 的收发接口的封装
     *      实现不同套接字通信的需求。SocketChannel 所有的接口都是委托给该类的派生类
     */
    class SocketChannelImp{
    private:
        /// @brief 写缓冲对象
        CPS::SocketDataBuffer inbuf;
        /// @brief 读缓冲对象
        CPS::SocketDataBuffer outbuf;
        /// @brief 远端地址，即目标通信地址
        CPS::SocketAddress remote_address;
        /// @brief 引用套接字管理器访问套接字文件描述符
        std::shared_ptr<SocketManager> manager;
    protected:
        /**
         * @brief  对资源指针进行断言，如果为空抛出异常
         * @details 
         *      引入Pimpl策略的缺点是有空指针访问的风险，该接口供内部其它接口调用对资源指针是否为空进行判断
         *      如果任何一个资源指针为空，就会抛出异常
         * @param msg 
        */
        void assert_nullptr(std::string msg);
    public:
        SocketChannelImp(){};
        SocketChannelImp(const SocketChannel& )=delete;
        SocketChannelImp(SocketChannel&& )=delete;

        virtual ~SocketChannelImp() = default;

        inline void setAddress(const CPS::SocketAddress& addr){
            this->remote_address = addr;
        }
        
        inline void setManager(std::shared_ptr<SocketManager> mng){
            this->manager = mng;
        }

        inline std::shared_ptr<SocketManager> getManager(){
            return this->manager;
        }

        inline CPS::SocketAddress& getAddress(){
            return this->remote_address;
        }

        inline CPS::SocketDataBuffer& getInBuffer(){
            return this->inbuf;
        }

        inline CPS::SocketDataBuffer& getOutBuffer(){
            return this->outbuf;
        }
        /**
         * @brief 判断当前套接字通道是否可读
         * @brief
         *      客户端和服务端的TCP通信套接字一定是可读的（注意双方都等待对方发送会导致死锁），因为TCP是面向连接的，只要有套接字文件描述符就可以通信。
         *      UDP通信套接字的套接字通道也是可读的，只是在接受到对方数据之前，远端地址是未知的
         *      
         * 
         * @return true 
         * @return false 
         */
        inline bool isReadable()const{
            return this->manager != nullptr;
        }
        /**
         * @brief 判断当前套接字通道是否可写
         * @details
         *      TCP的客户端和服务端通信套接字都一定是可写的，因为TCP是面向连接到，只需要有套接字文件描述符就可以通信
         *      而UDP通信套接字在创建套接字通道时是不知道通信地址的（当然你可以手动添加地址），只能读取远端地址发送的数据
         * @return true 
         * @return false 
         */
        inline bool isWritable()const{
            return 
                this->remote_address.getImp() != nullptr && 
                this->remote_address.getAddrIn() != nullptr && 
                this->manager != nullptr;
        }

        /**
         * @brief 调用该接口会从远端接收数据存储到读缓冲区中
         * @return size_t 
         * @throw
         *      具体异常可以参照 AntonaStandard::CPS::SocketLibraryManager::receive() 和 AntonaStandard::CPS::SocketLibraryManager::receiveFrom()
         */
        virtual size_t read() = 0;
        /**
         * @brief 调用该接口会将写缓冲区中的数据发送到远端地址，发送成功会清除写缓冲区
         * 
         * @return size_t 
         * @throw
         *      具体异常可以参照 AntonaStandard::CPS::SocketLibraryManager::send() 和 AntonaStandard::CPS::SocketLibraryManager::sendTo()
         */
        virtual size_t write() = 0;
        
        /**
         * @brief 针对不同的套接字通道的需求对自身进行 ‘拷贝’ 后返回
         * @details
         *      UDP 套接字通道为了保证不同端通信互不干涉，需要进行深拷贝。而 TCP 套接字通道有且只处理
         *      一个连接的通信，需要进行浅拷贝。但是由于std::shared_ptr 引用计数的问题，std::shared_ptr
         *      如果想要共享引用计数它们之间的实例必须来源于同一个实例的拷贝构造，如果通过裸指针重新
         *      构造std::shared_ptr 实例，那么这个新实例由于和原来的实例不共享引用计数，可能导致悬空指针
         * 
         * @return std::shared_ptr<SocketChannelImp> 
         * @throw
         *      AntonaStandard::Globals::NullPointer_Error
         *      std::runtime_error
         */
        virtual std::shared_ptr<SocketChannelImp> copy(std::shared_ptr<SocketChannelImp> src){
            // 判断src 是否为空
            if(src == nullptr){
                throw AntonaStandard::Globals::NullPointer_Error("In SocketChannelImp::copy() argument src is a null pointer!");
            }
            // 判断src 的资源指针是否和 this 相等
            if(src.get() != this){
                throw std::runtime_error("In SocketChannelImp::copy() argument src is not the same as this!");
            }
            return src;
        }
    };

    /**
     * @brief 套接字通道对象，负责数据读写，由Socket派生类构造
     * @details
     *      使用了Pimpl策略，所有的功能都是委托给 SocketChannelImp 实现的
     * 
     */
    class SocketChannel{
    private:
        std::shared_ptr<SocketChannelImp> imp;
        void assert_nullptr(std::string msg)const;
    public:
        SocketChannel() = default;
        SocketChannel(std::shared_ptr<SocketChannelImp> imp):imp(imp){};
        SocketChannel(const SocketChannel& );
        SocketChannel(SocketChannel&&);
        virtual ~SocketChannel()=default;
        inline bool isReadable()const{
            this->assert_nullptr("SocketChannel::isReadable()");
            return this->imp->isReadable();
        }

        inline bool isWritable()const{
            this->assert_nullptr("SocketChannel::isWritable()");
            return this->imp->isWritable();
        }

        inline const CPS::SocketAddress& getAddress() const{
            this->assert_nullptr("SocketChannel::getAddress()");
            return this->imp->getAddress();
        }

        inline void setAddress(const CPS::SocketAddress& addr){
            this->assert_nullptr("SocketChannel::setAddress(const SocketAddress& addr);");
            this->imp->setAddress(addr);
        }

        inline CPS::SocketDataBuffer& getInBuffer(){
            this->assert_nullptr("SocketChannel::getInBuffer();");
            return this->imp->getInBuffer();
        }

        inline CPS::SocketDataBuffer& getOutBuffer(){
            this->assert_nullptr("SocketChannel::getOutBuffer();");
            return this->imp->getOutBuffer();
        }

        inline size_t read(){
            this->assert_nullptr("SocketChannel::read();");
            return this->imp->read();
        }

        inline size_t write(){
            this->assert_nullptr("SocketChannel::write();");
            return this->imp->write();
        }
    };

    /**
     * @brief TCP 套接字通道的实现类
     * 
     */
    class TCPSocketChannelImp:public SocketChannelImp{
    public:
        TCPSocketChannelImp():SocketChannelImp(){};
        TCPSocketChannelImp(const TCPSocketChannelImp&) = delete;
        TCPSocketChannelImp(TCPSocketChannelImp&& ) = delete;
        virtual ~TCPSocketChannelImp(){};
        virtual size_t read() override;
        virtual size_t write() override;
    };
    /**
     * @brief UDP 套接字通道的实现类
     * 
     */
    class UDPSocketChannelImp:public SocketChannelImp{
    public:
        UDPSocketChannelImp():SocketChannelImp(){};
        UDPSocketChannelImp(const UDPSocketChannelImp& ) = delete;
        UDPSocketChannelImp(UDPSocketChannelImp&& ) = delete;
        virtual ~UDPSocketChannelImp() = default;
        virtual size_t read() override;
        virtual size_t write() override;
        virtual std::shared_ptr<SocketChannelImp> copy(std::shared_ptr<SocketChannelImp>)override;

    };


}

#endif