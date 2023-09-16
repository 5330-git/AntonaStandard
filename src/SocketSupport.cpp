#include "MultiPlatformSupport/SocketSupport.h"

namespace AntonaStandard::MultiPlatformSupport{


    void Socket::setAddress(SocketAddress* addr) {
        if(addr == nullptr){
            throw AntonaStandard::Utilities::NullPointer_Error("Get Null pointer in Socket::setAddress");
        }
        this->address = addr;
    }

    Socket::Socket() { 
        this->socketid = 0; 
        this->address = nullptr;
    }

    Socket::Socket(Socket& socket) {
        this->address = socket.getAddress()->copy();
    }

    Socket::Socket(Socket&& socket) {
        this->address = socket.getAddress();
        socket.address = nullptr;
    }

    Socket::~Socket() {
        if(this->address){
            delete this->address;
            this->address = nullptr;
        }
    }


    SocketCommunication::SocketCommunication() {
        // Windows 平台需要加载套接字库,而linux平台不需要做任何操作
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            WSADATA wsaData;
            WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
    }

    SocketCommunication::~SocketCommunication() {
        // Windows 平台需要释放套接字库,而linux平台不需要做任何操作
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            WSACleanup();
        #endif
    }

    Socket SocketCommunication::createSocket(SocketProtocol protocol,SocketType type, 
        unsigned short port,const char* addr_str,int defualt) {            
            Socket socket;
            switch(protocol){
                case SocketProtocol::ipv4:
                    socket.setAddress(new SocketAddressV4);
                    socket.getAddress()->setAddr(addr_str,port);
                    break;
                case SocketProtocol::ipv6:
                    socket.setAddress(new SocketAddressV6);
                    socket.getAddress()->setAddr(addr_str,port);
                    break;
                default:
                    break;
            }
            socket.setSocketId(::socket(protocol, type, defualt));
            return socket;
    }

    Socket SocketCommunication::createSocket(SocketProtocol prot,
        SocketType type,unsigned short port,
        const std::string addr_str,int defualt) {
            return this->createSocket(prot,type,port,addr_str.c_str(),defualt);
    }
    void SocketCommunication::bindSocket(Socket& socket) {
        int error = bind(socket.getSocketId(), (sockaddr*)socket.getAddress()->getAddrIn(),
                            socket.getAddress()->getAddrInSize());
        if (error) {
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss << "bind error, error = " << error;
            throw std::runtime_error(ss.str());
        }
    }

    void SocketCommunication::listenSocket(Socket& socket) {
        int error = listen(socket.getSocketId(), 128);
        if (error) {
            
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"listen error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
    }

    Socket SocketCommunication::acceptSocket(Socket& ssocket) {
        Socket client;
        // 从服务端套接字 ssocket 拷贝地址对象，保证二者使用的协议相同
        client.setAddress(ssocket.getAddress()->copy());
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int caddr_len = client.getAddress()->getAddrInSize();
        #elif AntonaStandard_PLATFORM_LINUX
            socklen_t caddr_len = client.getAddress()->getAddrInSize();
        #endif
        
        // 调用accept 将客户端的信息写入Socket对象
        Socket::Socketid_t clientid = accept(ssocket.getSocketId(), (sockaddr*)client.getAddress()->getAddrIn(), &caddr_len);
        if (clientid < 0) {
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"accept error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
        client.setSocketId(clientid);
        return client;
    }

    void SocketCommunication::connectSocket(Socket& ssocket) {
        // 连接ssocket对应主机的套接字
        int error = ::connect(ssocket.getSocketId(), (sockaddr*)ssocket.getAddress()->getAddrIn(),
                            ssocket.getAddress()->getAddrInSize());
        if (error) {
            // result 不为0，返回错误
            std::stringstream ss;
            ss<<"connect error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
    }

    void SocketCommunication::closeSocket(Socket& socket) {
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            closesocket(socket.getSocketId());
        #elif AntonaStandard_PLATFORM_LINUX
            close(socket.getSocketId());
        #endif
    }

    void SocketCommunication::send(Socket& socket,SocketDataBuffer& data) {
        // Windows下需要检查长度是否超过INTMAX
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            if (data.sendable() > INT_MAX) {
                std::stringstream ss;
                ss<<"send data size is too large, size = "<<data.sendable();
                throw std::runtime_error(ss.str());
            }
        #endif
        int size_send = ::send(socket.getSocketId(), data.getBeginPos(), data.sendable(), 0);
        if (size_send == -1) {
            // error = -1，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"send error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
    }

    size_t SocketCommunication::receive(Socket& socket,SocketDataBuffer& data) {
        size_t size_accepted = recv(socket.getSocketId(), data.getBeginPos(), data.str().size(), 0);
        if (size_accepted < 0) {
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"receive error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
        data.resize(size_accepted);
        return size_accepted;
    }

    std::streambuf::int_type SocketDataBuffer::overflow(
        std::streambuf::int_type c) {
        size_t read_pos = this->gptr()-this->eback();
        // 将buffer扩大到原来的二倍
        size_t pre_size = this->buffer.size();
        this->buffer.resize(this->buffer.size()*2);
        // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
            // 写入缓冲区的指针pbase,pptr,epptr
        this->setp(&(buffer[pre_size-1]),&(buffer.back()));
            // 输出，读取缓冲区的指针eback,gptr,egptr
        this->setg(&(buffer.front()),&(buffer[read_pos]),this->pptr());

        // 字符c是溢出的，还未存到内存中，因此需要调用sputc将其放回内存中
        this->sputc(c);
        return c;
    }

    std::streambuf::int_type SocketDataBuffer::underflow() {
        if(this->gptr()>=this->pptr()){
            // 可读的缓冲区已经耗尽，只能等待可读区扩展后才能继续读取
            return EOF;
        }
        // 读取缓存区指针重定向(扩展读取上限)
        this->setg(this->eback(),this->gptr(),this->pptr());
        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::int_type SocketDataBuffer::pbackfail(
        std::streambuf::int_type c) {
        if(c != EOF){
            if(this->gptr()>this->eback()){
                // 有位置可放回
                this->gbump(-1);        // 将gptr移动到上一个位置
                *(this->gptr()) = c;
            }
            // EOF 会导致后续任何对istream的操作都无效，因此我们不主动返回EOF
        }
        return c;
    }

    int SocketDataBuffer::sync() {
        // 对于放置区来说，需要重置放置位置
        // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
            // 写入缓冲区的指针pbase,pptr,epptr
        this->setp(&(buffer.front()),&(buffer.back()));
            // 输出，读取缓冲区的指针eback,gptr,egptr
                // 即将可读的内容清空
        this->setg(&(buffer.front()),&(buffer.front()),this->pptr());
        // 刷新成功，返回0
        return 0;
    }

    SocketDataBuffer::SocketDataBuffer() {
		this->resize(128);
	}

    void* SocketAddress::getAddrIn() { 
        return this->addr_in;
    }

    SocketAddress::~SocketAddress(){};

    SocketAddressV4::SocketAddressV4() {
        this->addr_in = new sockaddr_in;
    }

    SocketAddressV4::SocketAddressV4(SocketAddressV4& addr) {
        if(this->addr_in){
            // 注意不能直接删除 void* 类型指针
            delete (sockaddr_in*)this->addr_in;
            this->addr_in = nullptr;
        }
        this->addr_in = new sockaddr_in;
        // 基于内存拷贝addr地址数据结构
        std::memcpy(this->addr_in,addr.getAddrIn(),sizeof(sockaddr_in));
    }

    SocketAddressV4::SocketAddressV4(SocketAddressV4&& addr) {
        // 右值对象直接交换sockaddr_in结构
        if(this->addr_in){
            // 注意不能直接删除 void* 类型指针
            delete (sockaddr_in*)this->addr_in;
            this->addr_in = nullptr;
        }
        this->addr_in = addr.addr_in;
        addr.addr_in = nullptr;
    }
    SocketAddressV4::~SocketAddressV4() {
        if(this->addr_in){
            // 注意不能直接删除 void* 类型指针
            delete (sockaddr_in*)this->addr_in;
            this->addr_in = nullptr;
        }
    }
    size_t SocketAddressV4::getAddrInSize() { 
        return sizeof(sockaddr_in);
    }
    void SocketAddressV4::setAddr(const char* ip, unsigned short port) {
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in;
        // 设置协议 ipv4
        this_addr_in->sin_family = SocketProtocol::ipv4;
        // 设置ip
        this_addr_in->sin_addr.s_addr = inet_addr(ip);
        // 设置端口
        this_addr_in->sin_port = htons(port);
    }
    std::string SocketAddressV4::getIp() { 
        char temp_ip[20];           // ipv4最长15个字符
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in;
        // 获取ip
        auto ret = inet_ntop(SocketProtocol::ipv4,
            &(this_addr_in->sin_addr),
            temp_ip,sizeof(temp_ip));

        if(ret == nullptr){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            throw std::runtime_error(std::string("SocketAddressV4::getIp() failed: ") +
                                    std::to_string(error));
        }
        
        return std::string(temp_ip); 
    }
    unsigned short SocketAddressV4::getPort() { 
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in;
        // 获取端口
        return ntohs(this_addr_in->sin_port);
    }
    SocketProtocol SocketAddressV4::getProtocol() {
        return SocketProtocol::ipv4;
    }

    SocketAddressV4* SocketAddressV4::copy() { 
        return new SocketAddressV4(*this);    
    }

    SocketAddressV6::SocketAddressV6() {
        this->addr_in = new sockaddr_in6;
    }

    SocketAddressV6::SocketAddressV6(SocketAddressV6& addr) {
        if(this->addr_in){
            delete (sockaddr_in6*)this->addr_in;
            this->addr_in = nullptr;
        }
        this->addr_in = new sockaddr_in6;
        std::memcpy(this->addr_in,addr.getAddrIn(),sizeof(sockaddr_in6));
    }

    SocketAddressV6::SocketAddressV6(SocketAddressV6&& addr) {
        if(this->addr_in){
            delete (sockaddr_in6*)this->addr_in;
            this->addr_in = nullptr;
        }
        this->addr_in = addr.addr_in;
        addr.addr_in = nullptr;
    }

    SocketAddressV6::~SocketAddressV6() {
        if(this->addr_in){
            delete (sockaddr_in6*)this->addr_in;
            this->addr_in = nullptr;
        }
    }

    size_t SocketAddressV6::getAddrInSize() { 
        return sizeof(sockaddr_in6);    
    }

    void SocketAddressV6::setAddr(const char* ip, unsigned short port) {
        // 首先转化addr_in，方便操作
        sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in;
        // 设置协议
        this_addr_in->sin6_family = SocketProtocol::ipv6;
        // 设置ip
        inet_pton(SocketProtocol::ipv6,ip,&(this_addr_in->sin6_addr));
        // 设置端口
        this_addr_in->sin6_port = htons(port);
    }

    std::string SocketAddressV6::getIp() { 
        char temp_ip[50];
        auto ret = inet_ntop(SocketProtocol::ipv6,
            &(((sockaddr_in6*)this->addr_in)->sin6_addr),
            temp_ip,sizeof(temp_ip));
        if(ret == nullptr){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            // 抛出异常
            throw std::runtime_error("SocketAddressV6::getIp() failed: " + 
            std::to_string(error));
        }
        return std::string(temp_ip);
    }

    unsigned short SocketAddressV6::getPort() { 
        // 首先转化addr_in 方便操作
        sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in;
        return ntohs(this_addr_in->sin6_port);
    }

    SocketProtocol SocketAddressV6::getProtocol() { 
        return SocketProtocol::ipv6;
    }

    SocketAddressV6* SocketAddressV6::copy() { 
        return new SocketAddressV6(*this); 
    }

    }  // namespace AntonaStandard::MultiPlatformSupport
