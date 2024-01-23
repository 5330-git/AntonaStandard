#include <Network/Socket.h>
#include <Network/Socket.hpp>
AntonaStandard::Network::Socket::SocketLibraryManager::SocketLibraryManager() {
    // Windows 平台需要加载套接字库,而linux平台不需要做任何操作
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        WSADATA wsaData;
        WSAStartup(MAKEWORD(2, 2), &wsaData);
    #endif
}

void AntonaStandard::Network::Socket::SocketLibraryManager::launch() {
    volatile static SocketLibraryManager instance; // 创建单例实例
}

AntonaStandard::Network::Socket::SocketLibraryManager::~SocketLibraryManager() {
// Windows 平台需要释放套接字库,而linux平台不需要做任何操作
#ifdef AntonaStandard_PLATFORM_WINDOWS
    WSACleanup();
#endif
}


void AntonaStandard::Network::Socket::bind(){
    int error = ::bind(this->fd, static_cast<sockaddr*>(this->address.getAddrIn().get()),
                            this->address.getAddrInSize());
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

void AntonaStandard::Network::Socket::connect() {
    int error = ::connect(this->fd, static_cast<sockaddr*>(this->address.getAddrIn().get()),
                                  this->address.getAddrInSize());
    if (error) {
        // result 不为0，返回错误
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #else
            int error = errno;
        #endif
        std::stringstream ss;
        ss << "connect error, error = " << error;
        throw std::runtime_error(ss.str());
    }
}

void AntonaStandard::Network::Socket::listen() {
    int error = ::listen(this->fd, SOMAXCONN); // 最大连接数由系统决定，通常是128或500
    if (error) { // result 不为0，返回错误
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #else
            int error = errno;
        #endif
        std::stringstream ss;
        ss << "listen error, error = " << error;
        throw std::runtime_error(ss.str());
    }
}

AntonaStandard::Network::SocketAddress AntonaStandard::Network::Socket::accept(SocketFD& remote_fd) {
    SocketAddress remote_address = this->address;
    socklen_t len = remote_address.getAddrInSize();
    remote_fd = ::accept(this->fd, static_cast<sockaddr*>(remote_address.getAddrIn().get()), &len);
    if (remote_fd == INVALID_SOCKET) { // result 为 INVALID_SOCKET，抛出异常
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #else
            int error = errno;
        #endif
        std::stringstream ss;
        ss << "accept error, error = " << error;
        throw std::runtime_error(ss.str());
    }
    return remote_address;
}

size_t AntonaStandard::Network::Socket::receive(SocketFD remote_fd,
                                                SocketDataBuffer& buffer) {
    size_t size_accepted = ::recv(remote_fd, buffer.receivingPos(), buffer.getReceivableSize(), 0);
    if (size_accepted < 0) {
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        std::stringstream ss;
        ss<<"receive error, error = '"<<error<<"' with target address = "<<this->address.getIP()<<":"<<this->address.getPort();
        throw std::runtime_error(ss.str());
    }
    buffer.movePutPos(size_accepted);
    return size_accepted;
}

size_t AntonaStandard::Network::Socket::send(SocketFD remote_fd,
                                            const SocketDataBuffer& buffer) {
    // Windows下需要检查长度是否超过INTMAX
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        if (buffer.getSendableSize() > INT_MAX) {
            std::stringstream ss;
            ss<<"send data size is too large, size = "<<buffer.getSendableSize();
            throw std::runtime_error(ss.str());
        }
    #endif
    size_t size_send = ::send(remote_fd, buffer.sendingPos(), buffer.getSendableSize(), 0);
    if (size_send == -1) {
        // error = -1，返回错误
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        std::stringstream ss;
        ss<<"send error, error = '"<<error<<"' with target address = "<<this->address.getIP()<<":"<<this->address.getPort();
        throw std::runtime_error(ss.str());
    }
    return size_send;
}

bool AntonaStandard::Network::Socket::isClosable(SocketFD fd) { 
    return fd != INVALID_SOCKET; 
}

AntonaStandard::Network::Socket::Socket(const SocketAddress& address,ProtocolType protocol):address(address){
    this->fd = ::socket(this->address.getIPType(), protocol, 0);
}

AntonaStandard::Network::Socket::Socket(SocketAddress&& address,
                                        ProtocolType protocol):address(std::move(address)) {
    this->fd = ::socket(this->address.getIPType(), protocol, 0);
}

AntonaStandard::Network::Socket::~Socket() {
    this->close();
}

void AntonaStandard::Network::Socket::swap(Socket& other){
    std::swap(this->fd, other.fd);
    std::swap(this->address, other.address);
}

AntonaStandard::Network::Socket::Socket(Socket&& other){
    this->swap(other);
}

const AntonaStandard::Network::Socket& AntonaStandard::Network::Socket::operator=(Socket&& other){
    this->swap(other);
    return *this;
}

size_t AntonaStandard::Network::Socket::receiveFrom(SocketAddress& remote_addr, SocketDataBuffer& buf){
    remote_addr = this->address;
    socklen_t len = remote_addr.getAddrInSize();
    size_t size_accepted = ::recvfrom(this->fd, buf.receivingPos(),buf.getReceivableSize(), 0,
                                    static_cast<sockaddr*>(remote_addr.getAddrIn().get()), &len);
    if (size_accepted < 0) {
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        std::stringstream ss;
        ss<<"receive error, error = '"<<error<<"' with target address = "<<remote_addr.getIP()<<":"<<remote_addr.getPort();
        throw std::runtime_error(ss.str());
    }
    buf.movePutPos(size_accepted);
    return size_accepted;
}

size_t AntonaStandard::Network::Socket::sendTo(const SocketAddress& remote_addr,
                                            const SocketDataBuffer& buf) {
    // Windows下需要检查长度是否超过INTMAX
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        if (buf.getSendableSize() > INT_MAX) {
            std::stringstream ss;
            ss<<"send data size is too large, size = "<<buf.getSendableSize();
            throw std::runtime_error(ss.str());
        }
    #endif
    size_t size_send = ::sendto(this->fd, buf.sendingPos(), buf.getSendableSize(), 0,
                                    static_cast<sockaddr*>(remote_addr.getAddrIn().get()), remote_addr.getAddrInSize());

    if (size_send == -1) {
        // error = -1，返回错误
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        std::stringstream ss;
        ss<<"In 'sendTo' send error, error = '"<<error<<"' with target address = "<<remote_addr.getIP()<<":"<<remote_addr.getPort();

        throw std::runtime_error(ss.str());
    }
    return size_send;
}

void AntonaStandard::Network::Socket::close() {
    if(this->isClosable(this->fd)){
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            closesocket(this->fd);
        #elif AntonaStandard_PLATFORM_LINUX
            close(this->fd);
        #endif
        this->fd = INVALID_SOCKET;
    }
}

void AntonaStandard::Network::Tcp::LocalSocket::close() {
    if(this->isClosable(this->remote_fd)){
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            closesocket(this->remote_fd);
        #elif AntonaStandard_PLATFORM_LINUX
            close(this->remote_fd);
        #endif
        this->remote_fd = INVALID_SOCKET;
    }
    this->Socket::close(); // 关闭本地socket
}

AntonaStandard::Network::SocketAddressV4Imp::SocketAddressV4Imp() {
    this->addr_in = std::make_shared<sockaddr_in>();
}

AntonaStandard::Network::SocketAddressV4Imp::SocketAddressV4Imp(
    SocketAddressV4Imp& addr) {
    this->addr_in = std::make_shared<sockaddr_in>();
    // 基于内存拷贝addr地址数据结构
    std::memcpy(this->addr_in.get(),addr.getAddrIn().get(),sizeof(sockaddr_in));
}

AntonaStandard::Network::SocketAddressV4Imp::SocketAddressV4Imp(
    SocketAddressV4Imp&& addr) {
    this->addr_in.reset();
    using std::swap;
    swap(this->addr_in,addr.addr_in);
}

size_t AntonaStandard::Network::SocketAddressV4Imp::getAddrInSize() {
    return sizeof(sockaddr_in);
}

void AntonaStandard::Network::SocketAddressV4Imp::setAddr(const char* ip,
                                                        unsigned short port) {
    // 首先将addr_in进行转换，方便操作
    sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
    // 设置协议 ipv4
    this_addr_in->sin_family = IPType::ipv4;
    // 设置ip
    this_addr_in->sin_addr.s_addr = inet_addr(ip);
    // 设置端口
    this_addr_in->sin_port = htons(port);
}

std::string AntonaStandard::Network::SocketAddressV4Imp::getIP() {
    char temp_ip[20];           // ipv4最长15个字符
    // 首先将addr_in进行转换，方便操作
    sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
    // 获取ip
    auto ret = inet_ntop(IPType::ipv4,
        &(this_addr_in->sin_addr),
        temp_ip,sizeof(temp_ip));

    if(ret == nullptr){
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        throw std::runtime_error(std::string("SocketAddressV4::getIP() failed: ") +
                                std::to_string(error));
    }
    
    return std::string(temp_ip); 
}

unsigned short AntonaStandard::Network::SocketAddressV4Imp::getPort() {
    // 首先将addr_in进行转换，方便操作
    sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
    // 获取端口
    return ntohs(this_addr_in->sin_port);
}

AntonaStandard::Network::IPType AntonaStandard::Network::SocketAddressV4Imp::getIPType() {
    return IPType::ipv4;
}

std::shared_ptr<AntonaStandard::Network::SocketAddressImp>
AntonaStandard::Network::SocketAddressV4Imp::copy() {
    return std::make_shared<SocketAddressV4Imp>(*this);
}

AntonaStandard::Network::SocketAddressV6Imp::SocketAddressV6Imp() {
    this->addr_in = std::shared_ptr<sockaddr_in6>();
}

AntonaStandard::Network::SocketAddressV6Imp::SocketAddressV6Imp(
    SocketAddressV6Imp& addr) {
    this->addr_in = std::make_shared<sockaddr_in6>();
    std::memcpy(this->addr_in.get(),addr.getAddrIn().get(),sizeof(sockaddr_in6));
}

AntonaStandard::Network::SocketAddressV6Imp::SocketAddressV6Imp(
    SocketAddressV6Imp&& addr) {
    this->addr_in.reset();
    using std::swap;
    swap(this->addr_in,addr.addr_in);
}

size_t AntonaStandard::Network::SocketAddressV6Imp::getAddrInSize() {
    return sizeof(sockaddr_in6);
}

void AntonaStandard::Network::SocketAddressV6Imp::setAddr(const char* ip,
                                                        unsigned short port) {
    // 首先转化addr_in，方便操作
    sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in.get();
    // 设置协议
    this_addr_in->sin6_family = IPType::ipv6;
    // 设置ip
    inet_pton(IPType::ipv6,ip,&(this_addr_in->sin6_addr));
    // 设置端口
    this_addr_in->sin6_port = htons(port);
}

std::string AntonaStandard::Network::SocketAddressV6Imp::getIP() {
    char temp_ip[50];
    auto ret = inet_ntop(IPType::ipv6,
        &(((sockaddr_in6*)this->addr_in.get())->sin6_addr),
        temp_ip,sizeof(temp_ip));
    if(ret == nullptr){
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = WSAGetLastError();
        #elif AntonaStandard_PLATFORM_LINUX
            int error = errno;
        #endif
        // 抛出异常
        throw std::runtime_error("SocketAddressV6::getIP() failed: " + 
        std::to_string(error));
    }
    return std::string(temp_ip);
}

unsigned short AntonaStandard::Network::SocketAddressV6Imp::getPort() {
    // 首先转化addr_in 方便操作
    sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in.get();
    return ntohs(this_addr_in->sin6_port);
}

AntonaStandard::Network::IPType AntonaStandard::Network::SocketAddressV6Imp::getIPType() {
    return IPType::ipv6;
}

std::shared_ptr<AntonaStandard::Network::SocketAddressImp>
AntonaStandard::Network::SocketAddressV6Imp::copy() {
    return std::make_shared<SocketAddressV6Imp>(*this);
}

AntonaStandard::Network::SocketAddress::SocketAddress(IPType ip_type,unsigned short port,std::string ip){
    switch(ip_type){
        case IPType::ipv4:
            this->imp = std::make_shared<SocketAddressV4Imp>();
            this->imp->setAddr(ip.c_str(),port);
            break;
        case IPType::ipv6:
            this->imp = std::make_shared<SocketAddressV6Imp>();
            this->imp->setAddr(ip.c_str(),port);
            break;
        default:
            break;
    }
}

AntonaStandard::Network::SocketAddress::SocketAddress(const SocketAddress& other){
    this->imp = other.imp->copy();
}

void AntonaStandard::Network::SocketAddress::swap(SocketAddress& other) {
    using std::swap;
    swap(this->imp,other.imp);
}

const AntonaStandard::Network::SocketAddress& AntonaStandard::Network::SocketAddress::operator=(
    const SocketAddress& other) {
    SocketAddress temp(other);
    this->swap(temp); 
    return *this;
}

AntonaStandard::Network::SocketAddress::SocketAddress(SocketAddress&& other) {
    this->swap(other);
}

const AntonaStandard::Network::SocketAddress& AntonaStandard::Network::SocketAddress::operator=(
    SocketAddress&& other) {
    this->swap(other);
    return *this;
}

AntonaStandard::Network::SocketAddress AntonaStandard::Network::SocketAddress::loopBackAddress(
    unsigned short port, IPType ip_type) {
    switch(ip_type){
        case IPType::ipv4:
            return SocketAddress(IPType::ipv4,port,"127.0.0.1");
        case IPType::ipv6:
            return SocketAddress(IPType::ipv6,port,"::1");
        default:
            return SocketAddress(IPType::ipv4,port,"127.0.0.1");

    }
}

std::streambuf::int_type AntonaStandard::Network::SocketDataBuffer::overflow(
        std::streambuf::int_type c) {
        size_t read_pos = this->gptr()-this->eback();
        // 将buffer扩大到原来的二倍
        size_t put_pos = this->pptr()-this->pbase();
        size_t new_size = std::max(size_t(16),this->buffer.size()-1)*2+1;
        this->buffer.resize(new_size);
        // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
            // 写入缓冲区的指针pbase,pptr,epptr
        this->setp(&(buffer.front()),&(buffer.back()));
        this->pbump(put_pos);
        
            // 输出，读取缓冲区的指针eback,gptr,egptr
        this->setg(&(buffer.front()),&(buffer[read_pos]),this->pptr());

        // 字符c是溢出的，还未存到内存中，因此需要调用sputc将其放回内存中
        this->sputc(c);
        return c;
    }

    std::streambuf::int_type AntonaStandard::Network::SocketDataBuffer::underflow() {
        if(this->gptr()>=this->pptr()){
            // 可读的缓冲区已经耗尽，只能等待可读区扩展后才能继续读取
            return EOF;
        }
        // 读取缓存区指针重定向(扩展读取上限)
        this->setg(this->eback(),this->gptr(),this->pptr());
        return traits_type::to_int_type(*gptr());
    }

    std::streambuf::int_type AntonaStandard::Network::SocketDataBuffer::pbackfail(
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

    

    AntonaStandard::Network::SocketDataBuffer::SocketDataBuffer() {
		this->resize(128);
    }

    AntonaStandard::Network::Socket::ProtocolType AntonaStandard::Network::Tcp::LocalSocket::getProtocol()
        const {
      return ProtocolType::Stream;
    }

    AntonaStandard::Network::Socket::ProtocolType AntonaStandard::Network::Tcp::RemoteSocket::getProtocol()
        const {
      return ProtocolType::Stream;
    }


    AntonaStandard::Network::Socket::ProtocolType AntonaStandard::Network::Udp::LocalSocket::getProtocol()
        const {
      return ProtocolType::Dgram;
    }

    AntonaStandard::Network::Socket::ProtocolType AntonaStandard::Network::Udp::RemoteSocket::getProtocol()
        const {
      return ProtocolType::Dgram;
    }
