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
#include "TestingSupport/TestingMessageMacro.h"

// 条件包含不同平台的套接字库
#ifdef AntonaStandard_PLATFORM_WINDOWS
    #include <WinSock2.h>
    #include <WS2tcpip.h>
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
    }
    // namespace MultiPlatformSupport{
    //     class SocketDataBuffer;     // 数据传输使用的流对象缓冲区
    //     class SocketAddress;    // 存储ip地址与协议的基类型
    //     class SocketAddressV4;  // 存储ipv4地址与协议
    //     class SocketAddressV6;  // 存储ipv6地址与协议
    //     class Socket;           // 封装套接字描述符以及ip地址
    //     class SocketCommunication;  // 封装套接字通信的方法
    // }
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

    // 套接字文件描述符类型
    using SocketFd = 
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        SOCKET
    #elif AntonaStandard_PLATFORM_LINUX
        int
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
    public:
        // 获得本地主机的SocketAddress
        static SocketAddress loopBackAddress(unsigned short port,AddressFamily ip_type=AddressFamily::ipv4);
        static SocketAddress emptyAddress();
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
        void bind(SocketFd listen_fd,const SocketAddress& local_addr);
        void listen(SocketFd listen_fd,int backlog=SOMAXCONN);
        void connect(SocketFd communication_fd,const SocketAddress& remote_addr);
        SocketFd accept(SocketFd listen_fd,SocketAddress& remote_addr);
        size_t receive(SocketFd communication_fd,SocketDataBuffer& buffer,int flags=0);
        size_t send(SocketFd communication_fd,const SocketDataBuffer& buffer,int flags=0);
        size_t receiveFrom(SocketFd communication_fd,SocketDataBuffer& buffer,SocketAddress& remote_addr,int flags=0);
        size_t sendTo(SocketFd communication_fd,const SocketDataBuffer& buffer,const SocketAddress& remote_addr,int flags=0);
        inline bool isClosable(SocketFd& fd){
            return fd!=INVALID_SOCKET;
        }
        void close(SocketFd& fd);
    };
}

// namespace AntonaStandard::MultiPlatformSupport{
//     enum SocketType{
//         Stream = SOCK_STREAM,       // 流式协议
//         Dgram = SOCK_DGRAM,          // 报文协议
//         INVALID = ~0
//     };

//     enum SocketProtocol{
//         ipv4 = AF_INET,           // ipv4
//         ipv6 = AF_INET6           // ipv6
//     };

//     class SocketAddress{
//         TESTING_MESSAGE
//     protected:
//         std::shared_ptr<void> addr_in;      // 注意一定要初始化指针
//     public:
//         virtual std::shared_ptr<void> getAddrIn();
//         virtual size_t getAddrInSize()=0;
//         virtual void setAddr(const char* ip,
//                                 unsigned short port)=0;
//         virtual std::string getIp()=0;
//         virtual unsigned short getPort()=0;
//         virtual SocketProtocol getProtocol()=0;
//         virtual ~SocketAddress();
//         virtual std::shared_ptr<SocketAddress> copy()=0;
//     };
//     class SocketAddressV4:public SocketAddress{
//     public:
//         SocketAddressV4();                  // 默认构造函数，给addr_in new 一个sockaddr_in
//         SocketAddressV4(SocketAddressV4& addr);           // 内部完成addr_in指针的交换
//         SocketAddressV4(SocketAddressV4&& addr);
//         ~SocketAddressV4();
//         // 重写获取sockaddr大小的函数
//         virtual size_t getAddrInSize()override;
//         // 重写setAddr设置地址字段
//         virtual void setAddr(const char* ip,
//                                 unsigned short port)override;
//         virtual std::string getIp()override;
//         virtual unsigned short getPort()override;
//         virtual SocketProtocol getProtocol()override;
//         virtual std::shared_ptr<SocketAddress> copy()override;
//     };
//     class SocketAddressV6:public SocketAddress{
//     public:
//         SocketAddressV6();                  // 默认构造函数，给addr_in new 一个sockaddr_in
//         SocketAddressV6(SocketAddressV6& addr);           
//         SocketAddressV6(SocketAddressV6&& addr);
//         ~SocketAddressV6();
//         // 重写获取sockaddr大小的函数
//         virtual size_t getAddrInSize()override;
//         // 重写setAddr设置地址字段
//         virtual void setAddr(const char* ip,
//                                 unsigned short port)override;
//         virtual std::string getIp()override;
//         virtual unsigned short getPort()override;
//         virtual SocketProtocol getProtocol()override;
//         virtual std::shared_ptr<SocketAddress> copy()override;
        
//     };

//     // 使用strIngbuffer
//     class SocketDataBuffer:public std::streambuf{
//         // 使用string作为底层的缓冲容器
//     private:
//         std::string buffer;
//         inline void movePutPos(size_t size){
//             this->pbump(size);
//         }
//     protected:
//         // 没有可以放置元素的位置
//         std::streambuf::int_type virtual overflow(std::streambuf::int_type c)override;
//         // 没有可读数据
//         virtual int_type underflow()override;

//         // 处理放回字符的问题
//         std::streambuf::int_type virtual pbackfail(std::streambuf::int_type c)override;
//         // // 遇到std::flush 和 std::endl时回刷新
//         // int virtual sync()override;  // 不需要刷新，应当允许endl进行换行
//     public:
//         friend class SocketCommunication;
//         SocketDataBuffer();
//         inline char* receivingPos()const{
//             // 获取发送的数据的起始位点
//             return this->pptr();
//         }
//         inline char* sendingPos()const{
//             return this->pbase();
//         }
        
//         inline size_t getReceivableSize()const{
//             return this->epptr()-this->pptr();
//         }
//         inline size_t getSendableSize()const{
//             return this->pptr()-this->pbase();
//         }
        
//         inline const std::string& str()const{
//             return this->buffer;
//         }
//         inline void resize(size_t size){
            
//             auto pptr_pos = std::min(size_t(this->pptr()-this->pbase()),size);
//             ++size;             // epptr() 的位置是不可放和不可读的 
//             auto gptr_pos = std::min(size_t(this->gptr()-this->eback()),pptr_pos);      // 防止溢出

//             this->buffer.resize(std::max(size_t(1),size));          // 要为epptr留出位置
//             this->setp(&(this->buffer.front()),&(this->buffer.back()));
//             this->pbump(pptr_pos);
//             this->setg(this->pbase(),this->pbase()+gptr_pos,this->pptr());
//         }
//         inline void clear(){
//             // 对于放置区来说，需要重置放置位置
//             // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
//                 // 写入缓冲区的指针pbase,pptr,epptr
//             this->setp(&(buffer.front()),&(buffer.back()));
//                 // 输出，读取缓冲区的指针eback,gptr,egptr
//                     // 即将可读的内容清空
//             this->setg(&(buffer.front()),&(buffer.front()),this->pptr());
//         }
//         inline size_t size()const{
//             return this->buffer.size();
//         }
//     };


//     class Socket{
//     public:
//         using Socketid_t = 
//         #ifdef AntonaStandard_PLATFORM_WINDOWS
//             SOCKET
//         #elif AntonaStandard_PLATFORM_LINUX
//             int
//         #endif
//         ;
//         friend class SocketCommunication;
//     private:
//         Socketid_t socketid = INVALID_SOCKET;
//         std::shared_ptr<SocketAddress> address;
//         SocketType type=SocketType::INVALID;
//         // 以下三个函数只有SocketCommunication 可见
//         Socket();
//         void setAddress(std::shared_ptr<SocketAddress> addr);
        
//         inline void setSocketId(Socketid_t skt_id){
//             this->socketid = skt_id;
//         }
//     public:
//         inline std::shared_ptr<SocketAddress> getAddress(){
//             return this->address;
//         }
//         inline SocketType getType(){
//             return this->type;
//         }
//     public:
//         inline Socketid_t getSocketId() const{
//             return socketid;
//         }
//         // 支持swap语义
//         void swap(Socket& rhs);
//         bool closable()const{
//             return this->socketid!=INVALID_SOCKET;
//         }
        
        
//         Socket(Socket& socket)=delete;
//         Socket& operator=(Socket& socket)=delete;

//         Socket& operator=(Socket&& socket);
//         Socket(Socket&& socket);
//         ~Socket();
//     };

//     class SocketCommunication{
//     private:
//         SocketCommunication();
//     public:
//         static SocketCommunication& get();
//         ~SocketCommunication();
//         Socket createSocket(SocketProtocol prot,
//             SocketType type,unsigned short port,
//             const char* addr_str,int defualt=0);        // 初始化套接字，即在内部分配套接字id
//         Socket createSocket(SocketProtocol prot,
//             SocketType type,unsigned short port,
//             const std::string addr_str,int defualt=0);   
//         void bindSocket(Socket& socket);        // （服务端）绑定套接字（端口信息和ip保存在Socket内部）
//         void listenSocket(Socket& socket);      // （服务端）监听套接字（等待客户端连接）
//         Socket acceptSocket(Socket& socket);    // （服务端）接受客户端连接
//         void connectSocket(Socket& socket);     // （客户端）连接服务器
//         void closeSocket(Socket& socket);       // （客户端/服务端）关闭套接字

//         void send(Socket& socket,SocketDataBuffer& data);         // TCP发送数据
//         void sendTo(Socket& ssocket, std::shared_ptr<SocketAddress>& ret_client_addr, SocketDataBuffer& data);           // UDP 发送数据
//         size_t receive(Socket& socket, SocketDataBuffer& data);                // TCP 接收数据
//         size_t receiveFrom(Socket& ssocket, std::shared_ptr<SocketAddress>& ret_client_addr, SocketDataBuffer& data);      // UDP接收数据
//     };
// };

#endif