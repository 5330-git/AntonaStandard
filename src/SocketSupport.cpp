#include "MultiPlatformSupport/SocketSupport.h"

namespace AntonaStandard::MultiPlatformSupport{

    
    void Socket::setProtocol(SocketProtocol protocol) {
        this->addr_in.sin_family = protocol;
    }

    void Socket::setPort(unsigned short port) {
        this->port_id = port;
        this->addr_in.sin_port = htons(port);
    }

    void Socket::setAddress(const char* address) {
        int ret = inet_pton(this->getAddrIn().sin_family,address,&this->addr_in.sin_addr);
        if(ret == 0 || ret == -1){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif  AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream s;
            s<<"inet_pton return :"<<ret<<" Address \""<<address<<"\"transport error: "<<error<<"\n";
            throw std::runtime_error(s.str());
        }
    }

    void Socket::setAddress(const std::string& address) {
        this->setAddress(address.c_str());
    } 

    Socket::Socket() {
        this->socketid = 0;
    }

    Socket::~Socket() {}


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
            socket.setSocketId(::socket(protocol, type, defualt));
            socket.setProtocol(protocol);
            socket.setPort(port);
            socket.setAddress(addr_str);
            return socket;
    }

    Socket SocketCommunication::createSocket(SocketProtocol prot,
        SocketType type,unsigned short port,
        const std::string addr_str,int defualt) {
            return this->createSocket(prot,type,port,addr_str.c_str(),defualt);
    }
    void SocketCommunication::bindSocket(Socket& socket) {
        int error = bind(socket.getSocketId(), (sockaddr*)&socket.getAddrIn(),
                            sizeof(socket.getAddrIn()));
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
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int caddr_len = sizeof(client.getAddrIn());
        #elif AntonaStandard_PLATFORM_LINUX
            socklen_t caddr_len = sizeof(client.getAddrIn());
        #endif
        
        // 调用accept 将客户端的信息写入Socket对象
        Socket::Socketid_t clientid = accept(ssocket.getSocketId(), (sockaddr*)&client.getAddrIn(), &caddr_len);
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
        int error = ::connect(ssocket.getSocketId(), (sockaddr*)&ssocket.getAddrIn(),
                            sizeof(ssocket.getAddrIn()));
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
}  // namespace AntonaStandard::MultiPlatformSupport
