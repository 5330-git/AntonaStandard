#ifndef SOCKETSUPPORT_H
#define SOCKETSUPPORT_H

/* Decoded by utf-8
*   v-0.0.0 2023/9/14 初步完成套接字库的封装
*
*/
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include <string>
#include <vector>
#include "Utilities/Exception.h"
#include <stdexcept>
#include <sstream>
#include <cstring>

// 条件包含不同平台的套接字库
#ifdef AntonaStandard_PLATFORM_WINDOWS
    #include <WinSock2.h>
    #include <WS2tcpip.h>
#elif AntonaStandard_PLATFORM_LINUX
    #include <unistd.h>
    #include <sys/types.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif


namespace AntonaStandard{
    namespace MultiPlatformSupport{
        class SocketDataBuffer;     // 数据传输使用的流对象缓冲区
        class SocketAddress;    // 存储ip地址与协议的基类型
        class SocketAddressV4;  // 存储ipv4地址与协议
        class SocketAddressV6;  // 存储ipv6地址与协议
        class Socket;           // 封装套接字描述符以及ip地址
        class SocketCommunication;  // 封装套接字通信的方法
    }
};

namespace AntonaStandard::MultiPlatformSupport{
    enum SocketType{
        Stream = SOCK_STREAM,       // 流式协议
        Dgram = SOCK_DGRAM          // 报文协议
    };

    enum SocketProtocol{
        ipv4 = AF_INET,           // ipv4
        ipv6 = AF_INET6           // ipv6
    };

    class SocketAddress{
    protected:
        void* addr_in=nullptr;      // 注意一定要初始化指针
    public:
        virtual void* getAddrIn();
        virtual size_t getAddrInSize()=0;
        virtual void setAddr(const char* ip,
                                unsigned short port)=0;
        virtual std::string getIp()=0;
        virtual unsigned short getPort()=0;
        virtual SocketProtocol getProtocol()=0;
        virtual ~SocketAddress();
        virtual SocketAddress* copy()=0;
    };
    class SocketAddressV4:public SocketAddress{
    public:
        SocketAddressV4();                  // 默认构造函数，给addr_in new 一个sockaddr_in
        SocketAddressV4(SocketAddressV4& addr);           // 内部完成addr_in指针的交换
        SocketAddressV4(SocketAddressV4&& addr);
        ~SocketAddressV4();
        // 重写获取sockaddr大小的函数
        virtual size_t getAddrInSize()override;
        // 重写setAddr设置地址字段
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        virtual std::string getIp()override;
        virtual unsigned short getPort()override;
        virtual SocketProtocol getProtocol()override;
        virtual SocketAddressV4* copy()override;
    };
    class SocketAddressV6:public SocketAddress{
    public:
        SocketAddressV6();                  // 默认构造函数，给addr_in new 一个sockaddr_in
        SocketAddressV6(SocketAddressV6& addr);           // 内部完成addr_in指针的交换
        SocketAddressV6(SocketAddressV6&& addr);
        ~SocketAddressV6();
        // 重写获取sockaddr大小的函数
        virtual size_t getAddrInSize()override;
        // 重写setAddr设置地址字段
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        virtual std::string getIp()override;
        virtual unsigned short getPort()override;
        virtual SocketProtocol getProtocol()override;
        virtual SocketAddressV6* copy()override;
        
    };

    // 使用strIngbuffer
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
        // 遇到std::flush 和 std::endl时回刷新
        int virtual sync()override;
    public:
        SocketDataBuffer();
        inline size_t sendable()const{
            // 获取可发送的数据长度
            return this->pptr()-this->pbase()+1;
        }
        inline char* getBeginPos()const{
            // 获取发送的数据的起始位点
            return this->pbase();
        }
        inline const std::string& str(){
            return this->buffer;
        }
        inline void resize(size_t size){
            this->buffer.resize(size);
            this->setp(&(this->buffer.front()),&(this->buffer.back()));
            this->setg(this->pbase(),this->pbase(),this->pptr());
        }
    };


    class Socket{
    public:
        using Socketid_t = 
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            SOCKET
        #elif AntonaStandard_PLATFORM_LINUX
            int
        #endif
        ;
    private:
        Socketid_t socketid;
        SocketAddress* address;
    public:
        inline SocketAddress* getAddress(){
            return this->address;
        }
        void setAddress(SocketAddress* addr);
        
    public:
        inline Socketid_t getSocketId() const{
            return socketid;
        }
        inline void setSocketId(Socketid_t skt_id){
            this->socketid = skt_id;
        }
        Socket();
        Socket(Socket& socket);
        Socket(Socket&& socket);
        ~Socket();
    };

    class SocketCommunication{
    public:
        SocketCommunication();
        ~SocketCommunication();
        Socket createSocket(SocketProtocol prot,
            SocketType type,unsigned short port,
            const char* addr_str,int defualt=0);        // 初始化套接字，即在内部分配套接字id
        Socket createSocket(SocketProtocol prot,
            SocketType type,unsigned short port,
            const std::string addr_str,int defualt=0);   
        void bindSocket(Socket& socket);        // （服务端）绑定套接字（端口信息和ip保存在Socket内部）
        void listenSocket(Socket& socket);      // （服务端）监听套接字（等待客户端连接）
        Socket acceptSocket(Socket& socket);    // （服务端）接受客户端连接
        void connectSocket(Socket& socket);     // （客户端）连接服务器
        void closeSocket(Socket& socket);       // （客户端/服务端）关闭套接字

        void send(Socket& socket,SocketDataBuffer& data);         // 发送数据
        size_t receive(Socket& socket,SocketDataBuffer& data);      // 接收数据
    };
};
#endif