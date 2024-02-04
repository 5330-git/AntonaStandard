#ifndef CPS_SOCKET_H
#define CPS_SOCKET_H

/* Decoded by utf-8
*   v-0.0.0 2023/9/14 初步完成套接字库的封装
*   v-0.1.0 2023/10/24 健壮性改进
*   v-1.0.0 2023/11/4 增加UDP通信支持
*/
#include <CPS/Macros.h>
#include <string>
#include <vector>
#include <Globals/Exception.h>
#include <stdexcept>
#include <sstream>
#include <cstring>
#include <memory>
#include <algorithm>
#include <unordered_set>
#include <atomic>
#include "TestingSupport/TestingMessageMacro.h"

// 条件包含不同平台的套接字库
#ifdef AntonaStandard_PLATFORM_WINDOWS
    #include <WinSock2.h>
    #include <WS2tcpip.h>
    #include <unistd.h>
#elif AntonaStandard_PLATFORM_LINUX
    #include <unistd.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
    #define INVALID_SOCKET ~0
#endif

/**
 *   
 * 
  ____ ____  ____     ____             _        _     _     
 / ___|  _ \/ ___|_ _/ ___|  ___   ___| | _____| |_  | |__  
| |   | |_) \___ (_|_)___ \ / _ \ / __| |/ / _ \ __| | '_ \ 
| |___|  __/ ___) | _ ___) | (_) | (__|   <  __/ |_ _| | | |
 \____|_|   |____(_|_)____/ \___/ \___|_|\_\___|\__(_)_| |_|
                                                            
 * 该模块为套接字的简单封装
 */

namespace AntonaStandard{
    namespace CPS{
        class SocketAddress;
        class SocketAddressImp;
        class SocketAddressV4Imp;
        class SocketAddressV6Imp;

        class SocketDataBuffer;
        class SocketLibraryManager;
        class SocketManager;
    }
};

namespace AntonaStandard::CPS{
    enum AddressFamily{
        ipv4 = AF_INET,           // ipv4
        ipv6 = AF_INET6,          // ipv6
        invalid = ~0,
    };

    enum SocketType{
        Stream = SOCK_STREAM,       // 流式协议
        Datagram = SOCK_DGRAM,          // 报文协议
        InvalidType = ~0,
    };

    enum SocketProtocol{            // 对应首部的协议字段
        Default = 0,                // 默认协议
        TCP = IPPROTO_TCP,           // tcp协议等于6
        UDP = IPPROTO_UDP,           // udp协议等于17
        InvalidProtocol = ~0,
    };

    enum SocketOptions{            // 套接字选项
        // ReusePort = SO_REUSEPORT,     // 允许端口复用
        ReuseAddress = SO_REUSEADDR,   // 允许地址和端口复用
        // KeepAlive = SO_KEEPALIVE,       // 保持连接
        // NoDelay = TCP_NODELAY,         // 禁用Nagle算法
        /*其它选项后续用到再添加*/
        InvalidOption = ~0,
    };

    enum SocketLevel{
        Socket_level = SOL_SOCKET,          // 套接字选项
        IP_level = IPPROTO_IP,            // IP选项
        TCP_level = IPPROTO_TCP,           // TCP选项
        InvalidLevel = ~0,
    };

    enum ShutdownOptions{
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            Read = SD_RECEIVE,          // 关闭读通道
            Write = SD_SEND,             // 关闭写通道
            Both = SD_BOTH,              // 关闭读写通道
        #elif AntonaStandard_PLATFORM_LINUX
            Read = SHUT_RD,              // 关闭读通道
            Write = SHUT_WR,              // 关闭写通道
            Both = SHUT_RDWR,               // 关闭读写通道
        #endif
        InvalidShutdownOption = ~0,
    };


    // 套接字文件描述符类型
    using SocketFd = 
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        std::atomic<SOCKET>
    #elif AntonaStandard_PLATFORM_LINUX
        std::atomic<int>
    #endif
    ;

    class SocketAddressImp{
    protected:
        std::shared_ptr<void> addr_in;
    public:
        SocketAddressImp()=default;
        inline std::shared_ptr<void> getAddrIn()const{
            return addr_in;
        }
        virtual size_t getAddrInSize()const;
        virtual void setAddr(const char* ip,
                                unsigned short port);
        virtual std::string getIP()const;
        virtual unsigned short getPort()const;
        virtual AddressFamily getAddressFamily()const;
        virtual ~SocketAddressImp()=default;
        virtual std::shared_ptr<SocketAddressImp> copy();

        static std::shared_ptr<SocketAddressImp> create(const sockaddr* addr,socklen_t addr_len);
    };

    class SocketAddressV4Imp:public SocketAddressImp{
    public:
        SocketAddressV4Imp();                  // 默认构造函数，给addr_in new 一个sockaddr_in
        SocketAddressV4Imp(SocketAddressV4Imp& addr);           // 内部完成addr_in指针的交换
        SocketAddressV4Imp(SocketAddressV4Imp&& addr);
        ~SocketAddressV4Imp()=default;
        // 重写获取sockaddr大小的函数
        virtual size_t getAddrInSize()const override;
        // 重写setAddr设置地址字段
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        virtual std::string getIP()const override;
        virtual unsigned short getPort()const override;
        virtual AddressFamily getAddressFamily()const override;

        virtual std::shared_ptr<SocketAddressImp> copy()override;
    };

    class SocketAddressV6Imp:public SocketAddressImp{
    public:
        SocketAddressV6Imp();                  // 默认构造函数，给addr_in new 一个sockaddr_in
        SocketAddressV6Imp(SocketAddressV6Imp& addr);           
        SocketAddressV6Imp(SocketAddressV6Imp&& addr);
        ~SocketAddressV6Imp()=default;
        // 重写获取sockaddr大小的函数
        virtual size_t getAddrInSize()const override;
        // 重写setAddr设置地址字段
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        virtual std::string getIP()const override;
        virtual unsigned short getPort()const override;
        virtual AddressFamily getAddressFamily()const override;

        virtual std::shared_ptr<SocketAddressImp> copy()override;
    };

    class SocketAddress{
    private:
        std::shared_ptr<SocketAddressImp> imp;
    public: 
        SocketAddress(AddressFamily ip_type,std::string ip,unsigned short port);
        SocketAddress(const SocketAddress&);
        void swap(SocketAddress&);
        const SocketAddress& operator=(const SocketAddress&);
        SocketAddress(SocketAddress&&);
        const SocketAddress& operator=(SocketAddress&&);
        SocketAddress()=default;
        
        ~SocketAddress()=default;
        inline const std::shared_ptr<void> getAddrIn()const{
            return this->imp->getAddrIn();
        }
        inline socklen_t getAddrInSize()const{
            return this->imp->getAddrInSize();
        }
        inline AddressFamily getAddressFamily()const{
            return this->imp->getAddressFamily();
        }
        inline std::string getIP()const{
            return this->imp->getIP();
        }
        inline unsigned short getPort()const{
            return this->imp->getPort();
        }
        inline void setImp(std::shared_ptr<SocketAddressImp> imp){
            this->imp = imp;
        }
        inline const std::shared_ptr<SocketAddressImp> getImp()const{
            return this->imp;
        }
    public:
        // 获得本地主机的SocketAddress
        static SocketAddress loopBackAddress(unsigned short port,AddressFamily ip_type=AddressFamily::ipv4);
        static SocketAddress emptyAddress();
        static SocketAddress anyAddress(unsigned short port,AddressFamily ip_type=AddressFamily::ipv4);
    };

    class SocketDataBuffer:public std::streambuf{
        // 使用string作为底层的缓冲容器
    private:
        std::string buffer;
    protected:
        // 没有可以放置元素的位置
        std::streambuf::int_type virtual overflow(std::streambuf::int_type c)override;
        // 没有可读数据
        virtual int_type underflow()override;

        // 处理放回字符的问题
        std::streambuf::int_type virtual pbackfail(std::streambuf::int_type c)override;
        // // 遇到std::flush 和 std::endl时回刷新
        // int virtual sync()override;  // 不需要刷新，应当允许endl进行换行
    public:
        SocketDataBuffer();
        inline char* receivingPos()const{
            // 获取发送的数据的起始位点
            return this->pptr();
        }
        inline char* sendingPos()const{
            return this->pbase();
        }
        
        inline size_t getReceivableSize()const{
            return this->epptr()-this->pptr();
        }
        inline size_t getSendableSize()const{
            return this->pptr()-this->pbase();
        }
        
        inline const std::string& str()const{
            return this->buffer;
        }
        inline void resize(size_t size){
            
            auto pptr_pos = std::min(size_t(this->pptr()-this->pbase()),size);
            ++size;             // epptr() 的位置是不可放和不可读的 
            auto gptr_pos = std::min(size_t(this->gptr()-this->eback()),pptr_pos);      // 防止溢出

            this->buffer.resize(std::max(size_t(1),size));          // 要为epptr留出位置
            this->setp(&(this->buffer.front()),&(this->buffer.back()));
            this->pbump(pptr_pos);
            this->setg(this->pbase(),this->pbase()+gptr_pos,this->pptr());
        }
        inline void clear(){
            // 对于放置区来说，需要重置放置位置
            // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
                // 写入缓冲区的指针pbase,pptr,epptr
            this->setp(&(buffer.front()),&(buffer.back()));
                // 输出，读取缓冲区的指针eback,gptr,egptr
                    // 即将可读的内容清空
            this->setg(&(buffer.front()),&(buffer.front()),this->pptr());
        }
        inline size_t size()const{
            return this->buffer.size();
        }
        inline void movePutPos(size_t size){
            this->pbump(size);
        }
    };

    class SocketLibraryManager{
    private:
        SocketLibraryManager();
    public:
        SocketLibraryManager(const SocketLibraryManager&)=delete;
        SocketLibraryManager& operator=(const SocketLibraryManager&)=delete;
        SocketLibraryManager(SocketLibraryManager&&)=delete;
        SocketLibraryManager& operator=(SocketLibraryManager&&)=delete;
        ~SocketLibraryManager();
        static SocketLibraryManager& manager();

        // 操作套接字的接口和函数
        SocketFd socket(AddressFamily af_type=AddressFamily::ipv4,SocketType type=SocketType::Stream, SocketProtocol protocol=SocketProtocol::Default);
        void bind(const SocketFd& listen_fd,const SocketAddress& local_addr);
        void listen(const SocketFd& listen_fd,int backlog=SOMAXCONN);
        void connect(const SocketFd& communication_fd,const SocketAddress& remote_addr);
        SocketFd accept(const SocketFd& listen_fd,SocketAddress& remote_addr);
        size_t receive(const SocketFd& communication_fd,SocketDataBuffer& buffer,int flags=0);
        size_t send(const SocketFd& communication_fd,const SocketDataBuffer& buffer,int flags=0);
        size_t receiveFrom(const SocketFd& communication_fd,SocketDataBuffer& buffer,SocketAddress& remote_addr,int flags=0);
        size_t sendTo(const SocketFd& communication_fd,const SocketDataBuffer& buffer,const SocketAddress& remote_addr,int flags=0);
        inline bool isClosable(SocketFd& fd){
            return fd!=INVALID_SOCKET;
        }
        void close(SocketFd& fd);
        void shutdown(SocketFd& fd,ShutdownOptions how);
        void setSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,const void* optval,socklen_t optlen);
        void getSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,void* accept_optval,socklen_t* accept_optlen);
    };
}

#endif