#include <CPS/Socket.h>
#include <iostream>

namespace AntonaStandard::CPS{

    /***********   SocketAddressImp  **********
 ____             _        _      _       _     _                   ___                 
/ ___|  ___   ___| | _____| |_   / \   __| | __| |_ __ ___  ___ ___|_ _|_ __ ___  _ __  
\___ \ / _ \ / __| |/ / _ \ __| / _ \ / _` |/ _` | '__/ _ \/ __/ __|| || '_ ` _ \| '_ \ 
 ___) | (_) | (__|   <  __/ |_ / ___ \ (_| | (_| | | |  __/\__ \__ \| || | | | | | |_) |
|____/ \___/ \___|_|\_\___|\__/_/   \_\__,_|\__,_|_|  \___||___/___/___|_| |_| |_| .__/ 
                                                                                 |_|    
    */
    size_t SocketAddressImp::getAddrInSize() const {
        return size_t();
    }

    void SocketAddressImp::setAddr(const char* IP, unsigned short port){
        return ;
    }

    std::string SocketAddressImp::getIP()const{
        return std::string();
    }

    unsigned short SocketAddressImp::getPort()const{
        return 0;
    }

    AddressFamily SocketAddressImp::getAddressFamily()const{
        return AddressFamily::invalid;
    }
    
    std::shared_ptr<SocketAddressImp> SocketAddressImp::copy(){
        return std::shared_ptr<SocketAddressImp>();
    }

   
std::shared_ptr<SocketAddressImp> SocketAddressImp::create(const sockaddr * addr, socklen_t addr_len){
    // 通过addr_len 构造SocketAddressImp
    std::shared_ptr<SocketAddressImp> ret;
    switch(addr_len){
    case sizeof(sockaddr_in):
        ret = std::make_shared<SocketAddressV4Imp>();
        memcpy(ret->addr_in.get(), addr, addr_len);
        break;
    case sizeof(sockaddr_in6):
        ret = std::make_shared<SocketAddressV6Imp>();
        memcpy(ret->addr_in.get(), addr, addr_len);
        break;
    default:
        break;
    }
    return  ret; // 构造一个空的SocketAddressImp

} 

/************ SocketAddressV4Imp *******************
 ____             _        _      _       _     _                 __     ___  _  ___                 
/ ___|  ___   ___| | _____| |_   / \   __| | __| |_ __ ___  ___ __\ \   / / || ||_ _|_ __ ___  _ __  
\___ \ / _ \ / __| |/ / _ \ __| / _ \ / _` |/ _` | '__/ _ \/ __/ __\ \ / /| || |_| || '_ ` _ \| '_ \ 
 ___) | (_) | (__|   <  __/ |_ / ___ \ (_| | (_| | | |  __/\__ \__ \\ V / |__   _| || | | | | | |_) |
|____/ \___/ \___|_|\_\___|\__/_/   \_\__,_|\__,_|_|  \___||___/___/ \_/     |_||___|_| |_| |_| .__/ 
                                                                                              |_|        
     * ************************************************/
    

    SocketAddressV4Imp::SocketAddressV4Imp() {
        this->addr_in = std::make_shared<sockaddr_in>();
    }

    SocketAddressV4Imp::SocketAddressV4Imp(
        SocketAddressV4Imp& addr) {
        this->addr_in = std::make_shared<sockaddr_in>();
        // 基于内存拷贝addr地址数据结构
        std::memcpy(this->addr_in.get(),addr.getAddrIn().get(),sizeof(sockaddr_in));
    }

    SocketAddressV4Imp::SocketAddressV4Imp(
        SocketAddressV4Imp&& addr) {
        this->addr_in.reset();
        using std::swap;
        swap(this->addr_in,addr.addr_in);
    }

    size_t SocketAddressV4Imp::getAddrInSize()const {
        return sizeof(sockaddr_in);
    }

    void SocketAddressV4Imp::setAddr(const char* IP,
                                                            unsigned short port) {
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
        // 设置协议 ipv4
        this_addr_in->sin_family = AddressFamily::ipv4;
        // 设置IP
        this_addr_in->sin_addr.s_addr = inet_addr(IP);
        // 设置端口
        this_addr_in->sin_port = htons(port);
    }

    std::string SocketAddressV4Imp::getIP()const {
        char temp_IP[20];           // IPv4最长15个字符
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
        // 获取IP
        auto ret = inet_ntop(AddressFamily::ipv4,
            &(this_addr_in->sin_addr),
            temp_IP,sizeof(temp_IP));

        if(ret == nullptr){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            throw std::runtime_error(std::string("SocketAddressV4::getIP() failed: ") +
                                    std::to_string(error));
        }
        
        return std::string(temp_IP); 
    }

    unsigned short SocketAddressV4Imp::getPort()const {
        // 首先将addr_in进行转换，方便操作
        sockaddr_in* this_addr_in = (sockaddr_in*)this->addr_in.get();
        // 获取端口
        return ntohs(this_addr_in->sin_port);
    }

    AddressFamily SocketAddressV4Imp::getAddressFamily()const {
        return AddressFamily::ipv4;
    }

    std::shared_ptr<SocketAddressImp>
    SocketAddressV4Imp::copy() {
        return std::make_shared<SocketAddressV4Imp>(*this);
    }

    /************ SocketAddressV6Imp *******************
 ____             _        _      _       _     _                 __     ____  ___                 
/ ___|  ___   ___| | _____| |_   / \   __| | __| |_ __ ___  ___ __\ \   / / /_|_ _|_ __ ___  _ __  
\___ \ / _ \ / __| |/ / _ \ __| / _ \ / _` |/ _` | '__/ _ \/ __/ __\ \ / / '_ \| || '_ ` _ \| '_ \ 
 ___) | (_) | (__|   <  __/ |_ / ___ \ (_| | (_| | | |  __/\__ \__ \\ V /| (_) | || | | | | | |_) |
|____/ \___/ \___|_|\_\___|\__/_/   \_\__,_|\__,_|_|  \___||___/___/ \_/  \___/___|_| |_| |_| .__/ 
                                                                                            |_|    

     * ************************************************/

    SocketAddressV6Imp::SocketAddressV6Imp() {
        this->addr_in = std::make_shared<sockaddr_in6>();
    }

    SocketAddressV6Imp::SocketAddressV6Imp(
        SocketAddressV6Imp& addr) {
        this->addr_in = std::make_shared<sockaddr_in6>();
        std::memcpy(this->addr_in.get(),addr.getAddrIn().get(),sizeof(sockaddr_in6));
    }

    SocketAddressV6Imp::SocketAddressV6Imp(
        SocketAddressV6Imp&& addr) {
        this->addr_in.reset();
        using std::swap;
        swap(this->addr_in,addr.addr_in);
    }

    size_t SocketAddressV6Imp::getAddrInSize()const {
        return sizeof(sockaddr_in6);
    }

    void SocketAddressV6Imp::setAddr(const char* IP,
                                                            unsigned short port) {
        // 首先转化addr_in，方便操作
        sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in.get();
        // 设置协议
        this_addr_in->sin6_family = AddressFamily::ipv6;
        // 设置IP
        inet_pton(AddressFamily::ipv6,IP,&(this_addr_in->sin6_addr));
        // 设置端口
        this_addr_in->sin6_port = htons(port);
    }

    std::string SocketAddressV6Imp::getIP()const {
        char temp_IP[50];
        auto ret = inet_ntop(AddressFamily::ipv6,
            &(((sockaddr_in6*)this->addr_in.get())->sin6_addr),
            temp_IP,sizeof(temp_IP));
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
        return std::string(temp_IP);
    }

    unsigned short SocketAddressV6Imp::getPort()const {
        // 首先转化addr_in 方便操作
        sockaddr_in6* this_addr_in = (sockaddr_in6*)this->addr_in.get();
        return ntohs(this_addr_in->sin6_port);
    }

    AddressFamily SocketAddressV6Imp::getAddressFamily()const {
        return AddressFamily::ipv6;
    }

    std::shared_ptr<SocketAddressImp>
    SocketAddressV6Imp::copy() {
        return std::make_shared<SocketAddressV6Imp>(*this);
    }

    /***********   SocketAddress   **********
 ____             _        _      _       _     _                   
/ ___|  ___   ___| | _____| |_   / \   __| | __| |_ __ ___  ___ ___ 
\___ \ / _ \ / __| |/ / _ \ __| / _ \ / _` |/ _` | '__/ _ \/ __/ __|
 ___) | (_) | (__|   <  __/ |_ / ___ \ (_| | (_| | | |  __/\__ \__ \
|____/ \___/ \___|_|\_\___|\__/_/   \_\__,_|\__,_|_|  \___||___/___/
                                                                    
     * **************************************/
    SocketAddress::SocketAddress(AddressFamily ip_type,std::string ip,unsigned short port){
        switch(ip_type){
            case AddressFamily::ipv4:
                this->imp = std::make_shared<SocketAddressV4Imp>();
                this->imp->setAddr(ip.c_str(),port);
                break;
            case AddressFamily::ipv6:
                this->imp = std::make_shared<SocketAddressV6Imp>();
                this->imp->setAddr(ip.c_str(),port);
                break;
            default:
                this->imp = std::make_shared<SocketAddressImp>();
                break;
        }
    }

    SocketAddress::SocketAddress(const SocketAddress& other){
        this->imp = other.imp->copy();
    }

    void SocketAddress::swap(SocketAddress& other) {
        using std::swap;
        swap(this->imp,other.imp);
    }

    const SocketAddress& SocketAddress::operator=(
        const SocketAddress& other) {
        SocketAddress temp(other);
        this->swap(temp); 
        return *this;
    }

    SocketAddress::SocketAddress(SocketAddress&& other) {
        this->swap(other);
    }

    const SocketAddress& SocketAddress::operator=(
        SocketAddress&& other) {
        this->swap(other);
        return *this;
    }

    SocketAddress SocketAddress::loopBackAddress(
        unsigned short port, AddressFamily ip_type) {
        switch(ip_type){
            case AddressFamily::ipv4:
                return SocketAddress(AddressFamily::ipv4,"127.0.0.1",port);
            case AddressFamily::ipv6:
                return SocketAddress(AddressFamily::ipv6,"::1",port);
            default:
                return SocketAddress(AddressFamily::ipv4,"127.0.0.1",port);

        }
    }

    SocketAddress SocketAddress::emptyAddress(){
        SocketAddress addr;
        addr.imp = std::shared_ptr<SocketAddressImp>();
        return addr;
    }


 
SocketAddress SocketAddress::anyAddress(unsigned short port, AddressFamily ip_type){
    // ipv4: 0.0.0.0/0
    // ipv6: ::/0
    switch(ip_type){
        case AddressFamily::ipv4:
            return SocketAddress(AddressFamily::ipv4,"0.0.0.0",port);
        case AddressFamily::ipv6:
            return SocketAddress(AddressFamily::ipv6,"::",port);
        default:
            return SocketAddress(AddressFamily::ipv4,"0.0.0.0",port);
    }
}
   /***********   SocketDataBuffer   **********
 ____             _        _   ____        _        ____         __  __           
/ ___|  ___   ___| | _____| |_|  _ \  __ _| |_ __ _| __ ) _   _ / _|/ _| ___ _ __ 
\___ \ / _ \ / __| |/ / _ \ __| | | |/ _` | __/ _` |  _ \| | | | |_| |_ / _ \ '__|
 ___) | (_) | (__|   <  __/ |_| |_| | (_| | || (_| | |_) | |_| |  _|  _|  __/ |   
|____/ \___/ \___|_|\_\___|\__|____/ \__,_|\__\__,_|____/ \__,_|_| |_|  \___|_|   
                                                                    
     * **************************************/

    std::streambuf::int_type SocketDataBuffer::overflow(
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

    

    SocketDataBuffer::SocketDataBuffer() {
		this->resize(128);
	}

    /***********   SocketLibraryManager   **********
 ____             _        _   _     _ _                          __  __                                   
/ ___|  ___   ___| | _____| |_| |   (_) |__  _ __ __ _ _ __ _   _|  \/  | __ _ _ __   __ _  __ _  ___ _ __ 
\___ \ / _ \ / __| |/ / _ \ __| |   | | '_ \| '__/ _` | '__| | | | |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
 ___) | (_) | (__|   <  __/ |_| |___| | |_) | | | (_| | |  | |_| | |  | | (_| | | | | (_| | (_| |  __/ |   
|____/ \___/ \___|_|\_\___|\__|_____|_|_.__/|_|  \__,_|_|   \__, |_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|   
                                                            |___/                          |___/           
                                                                    
     * **************************************/

    SocketLibraryManager::SocketLibraryManager(){
            // Windows 平台需要加载套接字库,而linux平台不需要做任何操作
        #ifdef AntonaStandard_PLATFORM_WINDOWS
                WSADATA wsaData;
                WSAStartup(MAKEWORD(2, 2), &wsaData);
        #endif
    }

    SocketLibraryManager::~SocketLibraryManager(){
        // Windows 平台需要释放套接字库,而linux平台不需要做任何操作
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            WSACleanup();
        #endif
    }

    SocketLibraryManager& SocketLibraryManager::manager(){
        static SocketLibraryManager manager;
        return manager;
    }

    SocketFd SocketLibraryManager::socket(AddressFamily af_type,SocketType type, SocketProtocol protocol){
        auto fd = ::socket(af_type,type,protocol); // 创建套接字
        if(fd == INVALID_SOCKET){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            // 抛出异常
            throw std::runtime_error("SocketLibraryManager::socket() failed: " + std::to_string(error));
        }
        return fd;
    }

    void SocketLibraryManager::bind(const SocketFd& listen_fd,const SocketAddress& local_addr){
        int error = ::bind(listen_fd,static_cast<sockaddr*>(local_addr.getAddrIn().get()),local_addr.getAddrInSize());
        if (error) {
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss << "SocketLibraryManager::bind() error, error = " << error;
            throw std::runtime_error(ss.str());
        }
    }

    void SocketLibraryManager::listen(const SocketFd& listen_fd,int backlog){
        int error = ::listen(listen_fd, backlog);
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

    void SocketLibraryManager::connect(const SocketFd& communication_fd,const SocketAddress& remote_addr){
        int error = ::connect(communication_fd,static_cast<sockaddr*>(remote_addr.getAddrIn().get()),remote_addr.getAddrInSize());
        if (error) {
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss << "SocketLibraryManager::connect() error, error = " << error;
            throw std::runtime_error(ss.str());
        }
    }

    SocketFd SocketLibraryManager::accept(const SocketFd& listen_fd,SocketAddress& remote_addr){
        char addr_buf[64] = {0};
        sockaddr* temp = reinterpret_cast<sockaddr*>(addr_buf);
        socklen_t temp_len = sizeof(addr_buf);
        auto remote_fd = ::accept(listen_fd,temp,&temp_len);
        if (remote_fd == INVALID_SOCKET) {
            // result 不为0，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #else
                int error = errno;
            #endif
            std::stringstream ss;
            ss << "SocketLibraryManager::accept() error, error = " << error;
            throw std::runtime_error(ss.str());
        }
        // 根据sockaddr实际大小构造Imp
        auto addr_imp = SocketAddressImp::create(temp,temp_len);
        remote_addr.setImp(addr_imp);
        return remote_fd;
    }

    size_t SocketLibraryManager::receive(const SocketFd& communication_fd,SocketDataBuffer& buffer,int flags){
        size_t size_accepted = ::recv(communication_fd, buffer.receivingPos(), buffer.getReceivableSize(), flags);
        if (size_accepted == -1) {
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"receive error, error = "<<error;
            throw std::runtime_error(ss.str());
        }
        buffer.movePutPos(size_accepted);
        return size_accepted;       
    }

    size_t SocketLibraryManager::send(const SocketFd& communication_fd,const SocketDataBuffer& buffer,int flags){
        // Windows下需要检查长度是否超过INTMAX
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            if (buffer.getSendableSize() > INT_MAX) {
                std::stringstream ss;
                ss<<"send data size is too large, size = "<<buffer.getSendableSize();
                throw std::runtime_error(ss.str());
            }
        #endif
        size_t size_send = ::send(communication_fd, buffer.sendingPos(), buffer.getSendableSize(), flags);
        if (size_send == -1) {
            // error = -1，返回错误
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"send error, error = '"<<error<<'\'';
            throw std::runtime_error(ss.str());
        }
        return size_send;
    }

    size_t SocketLibraryManager::receiveFrom(const SocketFd& communication_fd,SocketDataBuffer& buffer,SocketAddress& remote_addr,int flags){
        /***
         * 对于udp客户端和服务端来说 communication_fd 对应本地使用socket函数创建的套接字
         * 
         * 
        */
        char addr_buf[64]={0};
        sockaddr* temp_addr = reinterpret_cast<sockaddr*>(addr_buf);
        socklen_t len = sizeof(addr_buf);
        size_t size_accepted = ::recvfrom(communication_fd, buffer.receivingPos(), buffer.getReceivableSize(),
                                        flags, temp_addr, &len);
        if (size_accepted == -1) {
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                int error = errno;
            #endif
            std::stringstream ss;
            ss<<"receive error, error = '"<<error<<"' with target address = "<<remote_addr.getIP()<<":"<<remote_addr.getPort();
            throw std::runtime_error(ss.str());
        }
        remote_addr.setImp(SocketAddressImp::create(temp_addr, len));

        buffer.movePutPos(size_accepted);
        return size_accepted;
    }

    size_t SocketLibraryManager::sendTo(const SocketFd& communication_fd,const SocketDataBuffer& buffer,const SocketAddress& remote_addr,int flags){
        /***
         * 对于udp客户端和服务端来说 communication_fd 对应本地使用socket函数创建的套接字
         * 
         * 
        */

        // Windows下需要检查长度是否超过INTMAX
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            if (buffer.getSendableSize() > INT_MAX) {
                std::stringstream ss;
                ss<<"send data size is too large, size = "<<buffer.getSendableSize();
                throw std::runtime_error(ss.str());
            }
        #endif
        size_t size_send = ::sendto(communication_fd, buffer.sendingPos(), buffer.getSendableSize(), flags
            ,static_cast<sockaddr*>(remote_addr.getAddrIn().get()), remote_addr.getAddrInSize());

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

    

    void SocketLibraryManager::close(SocketFd& fd){
        if(this->isClosable(fd)){
            
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = closesocket(fd);
            #elif AntonaStandard_PLATFORM_LINUX
                int error = ::close(fd);
            #endif
            // 保证写入操作排在前面
            fd.store(INVALID_SOCKET,std::memory_order_release);
            if(error == -1){
                #ifdef AntonaStandard_PLATFORM_WINDOWS
                    error = WSAGetLastError();
                #elif AntonaStandard_PLATFORM_LINUX
                    error = errno;
                #endif
                std::stringstream ss;
                ss<<"In 'close' close error, error = '"<<error<<"' with socket fd = "<<fd;
                throw std::runtime_error(ss.str());
            }
        }
    }
    void SocketLibraryManager::shutdown(SocketFd& fd, ShutdownOptions how) {
        if(this->isClosable(fd)){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                int error = ::shutdown(fd, how);
            #elif AntonaStandard_PLATFORM_LINUX
                int error = ::shutdown(fd, how);
            #endif
            if(error == -1){
                #ifdef AntonaStandard_PLATFORM_WINDOWS
                    error = WSAGetLastError();
                #elif AntonaStandard_PLATFORM_LINUX
                    error = errno;
                #endif
                std::stringstream ss;
                ss<<"In 'shutdown' shutdown error, error = '"<<error<<"' with socket fd = "<<fd;
                throw std::runtime_error(ss.str());
            }
        }
        // 如果双向都关闭将fd置为~0
        if(how == ShutdownOptions::Both){
            if(this->isClosable(fd)){
                fd.store(INVALID_SOCKET,std::memory_order_release);
            }
        }
    }

    void SocketLibraryManager::setSocketOption(const SocketFd& fd, SocketLevel level,
                                            SocketOptions option,
                                            const void* optval,
                                            socklen_t optlen) {
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = ::setsockopt(fd,level,option,static_cast<const char*>(optval),optlen);
        #elif AntonaStandard_PLATFORM_LINUX
            int error = ::setsockopt(fd,level,option,optval,optlen);
        #endif
        
        if(error == -1){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                error = errno;
            #endif
            std::stringstream ss;
            ss<<"In 'setSocketOption' error = '"<<error<<"' with socket fd = "<<fd;
            throw std::runtime_error(ss.str());
        }
    }

    void SocketLibraryManager::getSocketOption(const SocketFd& fd, SocketLevel level,
                                            SocketOptions option,
                                            void* accept_optval,
                                            socklen_t* accept_optlen) {
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            int error = ::getsockopt(fd,level,option,static_cast<char*>(accept_optval),accept_optlen);
        #elif AntonaStandard_PLATFORM_LINUX
            int error = ::getsockopt(fd,level,option,accept_optval,accept_optlen);
        #endif

        if(error == -1){
            #ifdef AntonaStandard_PLATFORM_WINDOWS
                error = WSAGetLastError();
            #elif AntonaStandard_PLATFORM_LINUX
                error = errno;
            #endif
            std::stringstream ss;
            ss<<"In 'getSocketOption' error = '"<<error<<"' with socket fd = "<<fd;
            throw std::runtime_error(ss.str());
        }
    }
}  // namespace AntonaStandard::CPS