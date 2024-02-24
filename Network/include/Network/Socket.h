#ifndef NETWORK_SOCKET_H
#define NETWORK_SOCKET_H

#include <CPS/Socket.h>
#include <Globals/Exception.h>
#include <mutex>
#include <atomic>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <iostream>

namespace AntonaStandard{
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

        void close();
        ~SocketManager();
    };

    class Socket{
    private:
        std::shared_ptr<SocketManager> manager;
        std::shared_ptr<SocketChannelImp> prototype;
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