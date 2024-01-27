// #ifndef NETWORK_SOCKET_H
// #define NETWORK_SOCKET_H

// #include <CPS/Macros.h>
// #include <memory>
// #include <cstring>
// #include <streambuf>
// #include <string>
// #include <sstream>
// // 条件包含不同平台的套接字库
// #ifdef AntonaStandard_PLATFORM_WINDOWS
//     #include <WinSock2.h>
//     #include <WS2tcpip.h>
// #elif AntonaStandard_PLATFORM_LINUX
//     #include <unistd.h>
//     #include <sys/types.h>
//     #include <arpa/inet.h>
//     #include <sys/socket.h>
//     #define INVALID_SOCKET ~0
// #endif

// namespace AntonaStandard{
//     namespace Network{
//         class SocketAddress;
//         class SocketAddressImp;
//         class SocketAddressV4Imp;
//         class SocketAddressV6Imp;
//         // 数据缓冲
//         class SocketDataBuffer;
//         // 套接字类
//         class Socket;     
//         namespace Tcp{
//             class LocalSocket;
//             class RemoteSocket;
//         }

//         namespace Udp{
//             class DatagramSocket;
//         }               
//     }
// }


// namespace AntonaStandard::Network{
//     enum IPType{
//         ipv4 = AF_INET,           // ipv4
//         ipv6 = AF_INET6,           // ipv6
//         INVALID_IP = AF_UNSPEC,    // 无效的IP类型
//     };    
    

//     class SocketDataBuffer:public std::streambuf{
//         // 使用string作为底层的缓冲容器
//     private:
//         std::string buffer;
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
//         inline void movePutPos(size_t size){
//             this->pbump(size);
//         }
//     };

//     class Socket{
//     public:
//         // Windows下套接字通信库管理器，负责ws_32加载和释放工作
//         class SocketLibraryManager{
//         private:
//             SocketLibraryManager();
//         public:
//             static void launch();
//             ~SocketLibraryManager();    
//         };
//         enum ProtocolType{
//             Stream = SOCK_STREAM,       // 流式协议
//             Dgram = SOCK_DGRAM,          // 报文协议
//             INVALID = ~0
//         };
        
//         // 套接字文件描述符类型
//         using SocketFD = 
//         #ifdef AntonaStandard_PLATFORM_WINDOWS
//             SOCKET
//         #elif AntonaStandard_PLATFORM_LINUX
//             int
//         #endif
//         ;
//     private:
//         SocketAddress address;
//         SocketFD fd=INVALID_SOCKET;
//     protected:
//         // 以下几个函数是对底层套接字通信函数的简单封装（收集错误码，并抛出异常）
//         virtual void bind(SocketFD local_fd,const SocketAddress& local_address);
//         virtual void connect(SocketFD remote_fd,const SocketAddress& remote_address);
//         virtual void listen(SocketFD local_fd,int backlog);
//         // accept 返回remote_fd
//         virtual SocketFD accept(SocketFD local_fd,SocketAddress& remote_address);
//         // receive 和 send 使用的remote_fd 来源于accept 的返回值
//         virtual size_t receive(SocketFD remote_fd,SocketDataBuffer& buffer,int flags=0);
//         virtual size_t send(SocketFD remote_fd,const SocketDataBuffer& buffer,int flags=0);
//         virtual size_t receiveFrom(SocketFD src_fd,SocketDataBuffer& buffer,SocketAddress& remote_address);
//             // 对udp客户端来说 src_fd 对应的是远端服务器的fd
//             // 对于udp服务端来说 src_fd 对应的是自己的fd
//         virtual size_t sendTo(SocketFD remote_fd,const SocketDataBuffer& buffer,const SocketAddress& remote_address);
        
//         virtual bool isClosable(SocketFD fd);
//     public:
//         Socket()=delete;
//         explicit Socket(const SocketAddress& address,ProtocolType protocol = Stream);
//         explicit Socket(SocketAddress&& address,ProtocolType protocol = Stream);

//         virtual ~Socket();
//         // 禁止拷贝构造
//         Socket(const Socket&) = delete;
//         Socket& operator=(const Socket&) = delete;
//         // 设置移动构造
//         Socket(Socket&&);
//         const Socket& operator=(Socket&&);
//         // 设置swap语义
//         void swap(Socket&);
//         // 地址相关
//         inline const SocketAddress& getAddress() const{
//             return this->address;
//         }
//         // 通信相关
//         virtual size_t receiveFrom(SocketAddress& remote_addr,SocketDataBuffer& buf);
//         virtual size_t sendTo(const SocketAddress& remote_addr, const SocketDataBuffer& buf);
//         // 套接字管理相关
//         virtual void close();
//         inline SocketFD getFD()const{
//             return this->fd;
//         }
//         // 套接字状态相关
//         virtual ProtocolType getProtocol()const = 0;
//     };

//     namespace Tcp{
//         class RemoteSocket:public Socket{
//         public:
//             RemoteSocket(const SocketAddress& address):Socket(address,Stream){};
//             RemoteSocket(SocketAddress&& address):Socket(address,Stream){};

//             ~RemoteSocket()=default;
//             // 套接字操作相关
//             virtual void connect()override{
//                 this->Socket::connect();
//             }
            
//             virtual size_t sendTo(const SocketAddress& addr, const SocketDataBuffer& buf)override{
//                 return this->Socket::sendTo(this->getAddress(),buf);
//             }
//             // 基于自己的套接字fd收发
//             inline size_t receive(SocketDataBuffer& buf){
//                 return this->Socket::receive(this->getFD(),buf);
//             }
//             inline size_t send(const SocketDataBuffer& buf){
//                 return this->Socket::send(this->getFD(),buf);
//             }
//             virtual ProtocolType getProtocol()const override;
//         };

//         class LocalSocket:public Socket{
//         private:
//             SocketFD remote_fd = INVALID_SOCKET;
//         public:
//             LocalSocket(const SocketAddress& address):Socket(address,Stream){};
//             LocalSocket(SocketAddress&& address):Socket(address,Stream){};

//             ~LocalSocket()=default;


//             inline void bind(){
//                 this->Socket::bind();
//             }

//             inline void listen(){
//                 this->Socket::listen();
//             }

//             inline SocketAddress accept(){
//                 return this->Socket::accept(this->remote_fd);
//             }

//             inline void connect(){
//                 this->Socket::connect();
//             }
//             virtual void close()override;

//             // 基于accept 获得的套接字fd收发
//             inline size_t receive(SocketDataBuffer& buf){
//                 return this->Socket::receive(this->remote_fd,buf);
//             }

//             inline size_t send(const SocketDataBuffer& buf){
//                 return this->Socket::send(this->remote_fd,buf);
//             }

//             virtual ProtocolType getProtocol()const override;

//         };
//     }

//     namespace Udp{
//         class RemoteSocket:public Socket{
//         public:
//             RemoteSocket(const SocketAddress& address):Socket(address,Dgram){};
//             RemoteSocket(SocketAddress&& address):Socket(address,Dgram){};
//             ~RemoteSocket()=default;
//             virtual size_t sendTo(const SocketAddress& addr,const SocketDataBuffer& buf){
//                 return this->Socket::sendTo(this->getAddress(),buf);
//             }

//             virtual ProtocolType getProtocol()const override;

//         };

//         class LocalSocket:public Socket{
//         public:
//             LocalSocket(const SocketAddress& address):Socket(address,Dgram){}
//             LocalSocket(SocketAddress&& address):Socket(address,Dgram){}
//             ~LocalSocket()=default;
//             inline void bind(){
//                 this->Socket::bind();
//             }

//             virtual ProtocolType getProtocol()const override;

//         };

    
//     }
    
// }

// #endif