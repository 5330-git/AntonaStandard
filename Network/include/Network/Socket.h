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
     *      使用了 Pimpl 策略，有关套接字文件描述符的管理委托给了类 AntonaStandard::Network::SockeManager
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
         *      通信需求，getChannel 函数会根据这个成员构造 SocketChannel 对象
         * @see
         *      virtual SocketChannel Socket::getChannel() 
         *      用于通信的套接字需要地址对象用于指定通信目标
         */
        std::shared_ptr<SocketChannelImp> prototype;
        /**
         * @brief 套接字地址
         * @details
         *      不同类型的套接字
         */
        CPS::SocketAddress bind_or_remote_addr;
        std::shared_ptr<std::once_flag> flag;
    protected:
        virtual void launch_stuff() = 0;

        inline std::shared_ptr<SocketChannelImp> getPrototype(){
            return this->prototype;
        }

        void assert_nullptr(std::string msg);

        virtual void setOption(CPS::SocketOptions option,SocketOptionValue value);

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
        
        inline std::shared_ptr<SocketManager> getManager(){
            return this->manager;
        };

        inline void install_manager(std::shared_ptr<SocketManager> mng){
            this->manager = mng;
        }

        inline void install_prototype(std::shared_ptr<SocketChannelImp> proto){
            this->prototype = proto;
        }

        inline std::shared_ptr<std::once_flag> getOnceFlag(){
            return this->flag;
        }

        virtual SocketChannel getChannel() = 0;
        void launch();
        inline void close(){
            this->assert_nullptr("Socket::close()");
            this->manager->close();
        }

        inline CPS::SocketAddress& get_bind_or_remote_addr(){
            return this->bind_or_remote_addr;
        }
        void setReuseAddress(bool);
        bool isReuseAddress();

    };

    class TCPListenSocket:public Socket{
    protected:
        virtual void launch_stuff()override;
    public:
        TCPListenSocket() = delete;
        TCPListenSocket(const CPS::SocketAddress& addr):Socket(addr){};
        TCPListenSocket(CPS::SocketAddress&& addr):Socket(addr){};
        TCPListenSocket(const TCPListenSocket& sock):Socket(sock){};
        TCPListenSocket(TCPListenSocket&& sock):Socket(sock){};

        TCPSocket accept();
        virtual SocketChannel getChannel()override;
    };

    class TCPSocket:public Socket{
    protected:
        virtual void launch_stuff()override;
    public:
        TCPSocket() = delete;
        TCPSocket(const CPS::SocketAddress& addr):Socket(addr){};
        TCPSocket(CPS::SocketAddress&& addr):Socket(addr){};
        TCPSocket(const TCPSocket& sock):Socket(sock){};
        TCPSocket(TCPSocket&& sock):Socket(sock){};
        virtual SocketChannel getChannel()override;
    };

    class UDPSocket:public Socket{
    protected:
        virtual void launch_stuff()override;
    public:
        UDPSocket() = delete;
        UDPSocket(const CPS::SocketAddress& addr):Socket(addr){};
        UDPSocket(CPS::SocketAddress&& addr):Socket(addr){};
        UDPSocket(const UDPSocket& sock):Socket(sock){};
        UDPSocket(UDPSocket&& sock):Socket(sock){};
        virtual SocketChannel getChannel()override;
    };

    class SocketOptionValue{
    private:
        std::shared_ptr<void> val;
        socklen_t val_size;
    public:
        inline socklen_t size()const{
            return this->val_size;
        }

        inline const std::shared_ptr<void> value_ptr()const{
            return this->val;
        }

        template<typename T>
        SocketOptionValue(T val){
            this->val = std::make_shared<T>(val);
            this->val_size = sizeof(val);
        }

        void setValue(const void* val,socklen_t len);
        SocketOptionValue(const void* val,socklen_t len);
        SocketOptionValue(const SocketOptionValue& val);

    };

    class SocketChannelImp{
    private:
        CPS::SocketDataBuffer inbuf;
        CPS::SocketDataBuffer outbuf;
        CPS::SocketAddress remote_address;
        std::shared_ptr<SocketManager> manager;
    protected:
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

        inline bool isReadable()const{
            return this->manager != nullptr;
        }

        inline bool isWritable()const{
            return 
                this->remote_address.getImp() != nullptr && 
                this->remote_address.getAddrIn() != nullptr && 
                this->manager != nullptr;
        }

        virtual size_t read() = 0;
        virtual size_t write() = 0;
        virtual std::shared_ptr<SocketChannelImp> copy() = 0;
    };

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


    class TCPSocketChannelImp:public SocketChannelImp{
    public:
        TCPSocketChannelImp():SocketChannelImp(){};
        TCPSocketChannelImp(const TCPSocketChannelImp&) = delete;
        TCPSocketChannelImp(TCPSocketChannelImp&& ) = delete;
        virtual ~TCPSocketChannelImp(){};
        virtual size_t read() override;
        virtual size_t write() override;
        virtual std::shared_ptr<SocketChannelImp> copy()override;
    };

    class UDPSocketChannelImp:public SocketChannelImp{
    public:
        UDPSocketChannelImp():SocketChannelImp(){};
        UDPSocketChannelImp(const UDPSocketChannelImp& ) = delete;
        UDPSocketChannelImp(UDPSocketChannelImp&& ) = delete;
        virtual ~UDPSocketChannelImp() = default;
        virtual size_t read() override;
        virtual size_t write() override;
        virtual std::shared_ptr<SocketChannelImp> copy()override;

    };


}

#endif