#include <Network/Socket.h>
// #include "Socket.h"
namespace AntonaStandard::Network{
    /***********Socket*******************
     * 
     * 
 ____             _        _   
/ ___|  ___   ___| | _____| |_ 
\___ \ / _ \ / __| |/ / _ \ __|
 ___) | (_) | (__|   <  __/ |_ 
|____/ \___/ \___|_|\_\___|\__|
                               
     * 
     * 
     */
    void Socket::assert_nullptr(
            std::string msg){
        if(
            !this->manager || 
            !this->prototype ||
            !this->bind_or_remote_addr.getImp() ||
            !this->flag){
            throw AntonaStandard::Globals::NullPointer_Error(msg.c_str());
        }
    } 

    void Socket::setOption(CPS::SocketOptions option, SocketOptionValue value){
        this->assert_nullptr("Socket::setOption(CPS::SocketOptions,SocketOptionValue)");
        CPS::SocketLibraryManager::manager().setSocketOption(
            this->manager->getSocketFd(),
            CPS::SocketLevel::Socket_level,
            option,
            value.value_ptr().get(),
            value.size()
        );
    }

    SocketOptionValue Socket::getOption(CPS::SocketOptions option) {
        this->assert_nullptr("Socket::getOption(CPS::SocketOptions);");
        
        char val_buf[64] = {0};
        socklen_t len = sizeof(val_buf);

        CPS::SocketLibraryManager::manager().getSocketOption(
            this->manager->getSocketFd(),
            CPS::SocketLevel::Socket_level,
            option,
            val_buf,
            &len
        );

        return SocketOptionValue(val_buf,len);
    }

    Socket::Socket(const Socket& other) {
        this->manager = other.manager;
        this->prototype = other.prototype;
        this->bind_or_remote_addr = other.bind_or_remote_addr;
        this->flag = other.flag;
    }

    Socket::Socket(Socket&& other) {
        using std::swap;
        swap(this->manager,other.manager);
        swap(this->prototype,other.prototype);
        swap(this->bind_or_remote_addr,other.bind_or_remote_addr);
        swap(this->flag,other.flag);
    }

    void Socket::launch() {
        std::call_once(*(this->flag),[this](){
            this->launch_stuff();
        });
    }

    void Socket::setReuseAddress(bool option) {
        SocketOptionValue val(option);
        this->setOption(
            CPS::SocketOptions::ReuseAddress,
            val
        );
    }

    bool Socket::isReuseAddress() { 
        auto val = this->getOption(CPS::SocketOptions::ReuseAddress);
        return *static_cast<bool*>(val.value_ptr().get());
    }

    SocketChannel Socket::getChannel(){
        return SocketChannel(this->prototype->copy(this->prototype));
    }

    /************TCPListenSocket********
     * 
 _____ ____ ____  _     _     _             ____             _        _   
|_   _/ ___|  _ \| |   (_)___| |_ ___ _ __ / ___|  ___   ___| | _____| |_ 
  | || |   | |_) | |   | / __| __/ _ \ '_ \\___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/| |___| \__ \ ||  __/ | | |___) | (_) | (__|   <  __/ |_ 
  |_| \____|_|   |_____|_|___/\__\___|_| |_|____/ \___/ \___|_|\_\___|\__|
     * 
     * 
    */


    void TCPListenSocket::launch_stuff(){
        // 创建套接字文件描述符
        CPS::SocketFd fd = CPS::SocketLibraryManager::manager().socket(
            this->get_bind_or_remote_addr().getAddressFamily(),
            CPS::SocketType::Stream,
            CPS::SocketProtocol::TCP
        );

        // 创建并安装套接字管理器
        auto new_manager = std::make_shared<SocketManager>(fd.load());
        this->install_manager(new_manager);
        
        // 创建并安装原型
        auto new_prototype = std::make_shared<TCPSocketChannelImp>();
        new_prototype->setAddress(this->get_bind_or_remote_addr());
        new_prototype->setManager(this->getManager());
        this->install_prototype(new_prototype);
        
        // 绑定地址
        CPS::SocketLibraryManager::manager().bind(
            fd,
            this->get_bind_or_remote_addr()
        );

        // 设置监听
        CPS::SocketLibraryManager::manager().listen(fd);
    }

    TCPSocket TCPListenSocket::accept() { 
        this->assert_nullptr("TCPListenSocket::accept();");

        CPS::SocketAddress remote_address;
        CPS::SocketFd fd = CPS::SocketLibraryManager::manager().accept(
            this->getManager()->getSocketFd(),
            remote_address
        );

        // 创建TCP套接字
        TCPSocket ret(remote_address);

        // 创建并安装套接字管理器
        auto new_manager = std::make_shared<SocketManager>(fd.load());
        ret.install_manager(new_manager);

        // 创建并安装原型
        auto new_prototype = std::make_shared<TCPSocketChannelImp>();
        new_prototype->setAddress(remote_address);
        new_prototype->setManager(new_manager);
        ret.install_prototype(new_prototype);
        
        // 设置once_flag 保证launch_stuff不会被调用
        std::call_once(
            *(ret.getOnceFlag().get()),
            [](){
                ;
            }
        );
        
        return ret;
    }

    SocketChannel TCPListenSocket::getChannel() { 
        // 禁用该函数（抛出异常）
        throw std::runtime_error("TCPListenSocket::getChannel() is not permited");
    }

    /************TCPSocket**************
     * 
 _____ ____ ____  ____             _        _   
|_   _/ ___|  _ \/ ___|  ___   ___| | _____| |_ 
  | || |   | |_) \___ \ / _ \ / __| |/ / _ \ __|
  | || |___|  __/ ___) | (_) | (__|   <  __/ |_ 
  |_| \____|_|   |____/ \___/ \___|_|\_\___|\__|
                                                
     * 
    */

    void TCPSocket::launch_stuff(){
        // 创建套接字文件描述符
        CPS::SocketFd fd = CPS::SocketLibraryManager::manager().socket(
            this->get_bind_or_remote_addr().getAddressFamily(),
            CPS::SocketType::Stream,
            CPS::SocketProtocol::TCP
        );

        // 创建并安装套接字管理器
        auto new_manager = std::make_shared<SocketManager>(fd.load());
        this->install_manager(new_manager);
        
        // 创建并安装原型
        auto new_prototype = std::make_shared<TCPSocketChannelImp>();
        new_prototype->setAddress(this->get_bind_or_remote_addr());
        new_prototype->setManager(this->getManager());
        this->install_prototype(new_prototype);

        // 连接到服务器
        CPS::SocketLibraryManager::manager().connect(
            fd,
            this->get_bind_or_remote_addr()
        );
    }


    /************UDPSocket***************
     * 
 _   _ ____  ____  ____             _        _   
| | | |  _ \|  _ \/ ___|  ___   ___| | _____| |_ 
| | | | | | | |_) \___ \ / _ \ / __| |/ / _ \ __|
| |_| | |_| |  __/ ___) | (_) | (__|   <  __/ |_ 
 \___/|____/|_|   |____/ \___/ \___|_|\_\___|\__|
    *
    */

    void UDPSocket::launch_stuff(){
        // 创建套接字文件描述符
        CPS::SocketFd fd = CPS::SocketLibraryManager::manager().socket(
            this->get_bind_or_remote_addr().getAddressFamily(),
            CPS::SocketType::Datagram,
            CPS::SocketProtocol::UDP
        );

        // 创建并安装套接字管理器
        auto new_manager = std::make_shared<SocketManager>(fd.load());
        this->install_manager(new_manager);
        
        // 创建并安装原型
        auto new_prototype = std::make_shared<UDPSocketChannelImp>();
        new_prototype->setAddress(this->get_bind_or_remote_addr());
        new_prototype->setManager(this->getManager());
        this->install_prototype(new_prototype);

        // 绑定到本地地址
        // 绑定地址
        CPS::SocketLibraryManager::manager().bind(
            fd,
            this->get_bind_or_remote_addr()
        );
    }


    /************SocketOptionValue******
     * 
     * 
 ____             _        _    ___        _   _          __     __    _            
/ ___|  ___   ___| | _____| |_ / _ \ _ __ | |_(_) ___  _ _\ \   / /_ _| |_   _  ___ 
\___ \ / _ \ / __| |/ / _ \ __| | | | '_ \| __| |/ _ \| '_ \ \ / / _` | | | | |/ _ \
 ___) | (_) | (__|   <  __/ |_| |_| | |_) | |_| | (_) | | | \ V / (_| | | |_| |  __/
|____/ \___/ \___|_|\_\___|\__|\___/| .__/ \__|_|\___/|_| |_|\_/ \__,_|_|\__,_|\___|
                                    |_|                                             
     * 
     * 
     * 
     */
    
    void SocketOptionValue::setValue(const void* val, socklen_t len) {
        char* new_val = new char[len];
        // 拷贝
        std::memcpy(new_val, val, len);
        this->val = std::shared_ptr<char>(new_val, [](char*& p) {
            delete[] p;
            p = nullptr;
        });
    }

    SocketOptionValue::SocketOptionValue(const void* val, socklen_t len) {
        this->setValue(val,len);
    }

    SocketOptionValue::SocketOptionValue(const SocketOptionValue& val) {
        this->setValue(val.val.get(),val.size());
    }

    /************SocketChannel*********
     * 
     * 
 ____             _        _    ____ _                            _ 
/ ___|  ___   ___| | _____| |_ / ___| |__   __ _ _ __  _ __   ___| |
\___ \ / _ \ / __| |/ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ |
 ___) | (_) | (__|   <  __/ |_| |___| | | | (_| | | | | | | |  __/ |
|____/ \___/ \___|_|\_\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|
                                                                    
     * 
     * 
    */

    void SocketChannel::assert_nullptr(std::string msg)const{
        if(!this->imp){
            throw Globals::NullPointer_Error(msg.c_str());
        }
    }

    SocketChannel::SocketChannel(const SocketChannel& other) {
        this->imp = other.imp->copy(other.imp);
    }

    SocketChannel::SocketChannel(SocketChannel&& other) {
        using std::swap;
        swap(this->imp,other.imp);
    }

    /************SocketChannelImp*********
     * 
     * 
 ____             _        _    ____ _                            _ ___                 
/ ___|  ___   ___| | _____| |_ / ___| |__   __ _ _ __  _ __   ___| |_ _|_ __ ___  _ __  
\___ \ / _ \ / __| |/ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ || || '_ ` _ \| '_ \ 
 ___) | (_) | (__|   <  __/ |_| |___| | | | (_| | | | | | | |  __/ || || | | | | | |_) |
|____/ \___/ \___|_|\_\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|___|_| |_| |_| .__/ 
                                                                                 |_|    
     * 
     * 
     * 
     * 
    */
    void SocketChannelImp::assert_nullptr(std::string msg){
        if(!this->manager){
            throw Globals::NullPointer_Error(msg.c_str());
        }    
    }

    /************TCPSocketChannelImp*******
     * 
 _____ ____ ____  ____             _        _    ____ _                            _ ___                 
|_   _/ ___|  _ \/ ___|  ___   ___| | _____| |_ / ___| |__   __ _ _ __  _ __   ___| |_ _|_ __ ___  _ __  
  | || |   | |_) \___ \ / _ \ / __| |/ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ || || '_ ` _ \| '_ \ 
  | || |___|  __/ ___) | (_) | (__|   <  __/ |_| |___| | | | (_| | | | | | | |  __/ || || | | | | | |_) |
  |_| \____|_|   |____/ \___/ \___|_|\_\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|___|_| |_| |_| .__/ 
                                                                                                  |_|    
     * 
     * 
    */

    size_t TCPSocketChannelImp::read(){
        this->assert_nullptr("TCPSocketChannelImp::read();");
        return CPS::SocketLibraryManager::manager().receive(
            this->getManager()->getSocketFd(),
            this->getInBuffer()
        );
    }

    size_t TCPSocketChannelImp::write() { 
        this->assert_nullptr("TCPSocketChannelImp::write();");
        auto send_size = CPS::SocketLibraryManager::manager().send(
            this->getManager()->getSocketFd(),
            this->getOutBuffer()
        );
        // 清空写缓冲
        this->getOutBuffer().clear();
        return send_size;
    }


    /************UDPSocketChannelImp*****
     * 
 _   _ ____  ____  ____             _        _    ____ _                            _ ___                 
| | | |  _ \|  _ \/ ___|  ___   ___| | _____| |_ / ___| |__   __ _ _ __  _ __   ___| |_ _|_ __ ___  _ __  
| | | | | | | |_) \___ \ / _ \ / __| |/ / _ \ __| |   | '_ \ / _` | '_ \| '_ \ / _ \ || || '_ ` _ \| '_ \ 
| |_| | |_| |  __/ ___) | (_) | (__|   <  __/ |_| |___| | | | (_| | | | | | | |  __/ || || | | | | | |_) |
 \___/|____/|_|   |____/ \___/ \___|_|\_\___|\__|\____|_| |_|\__,_|_| |_|_| |_|\___|_|___|_| |_| |_| .__/ 
                                                                                                   |_|    

    */

    size_t UDPSocketChannelImp::read(){
        this->assert_nullptr("UDPSocketChannelImp::read();");
        return CPS::SocketLibraryManager::manager().receiveFrom(
            this->getManager()->getSocketFd(),
            this->getInBuffer(),
            this->getAddress()
        );
    }

    size_t UDPSocketChannelImp::write() { 
        this->assert_nullptr("UDPSocketChannelImp::write();");
        auto send_size = CPS::SocketLibraryManager::manager().sendTo(
            this->getManager()->getSocketFd(),
            this->getOutBuffer(),
            this->getAddress()
        );
        // 清空写缓冲
        this->getOutBuffer().clear();
        return send_size;
    }

    std::shared_ptr<SocketChannelImp> UDPSocketChannelImp::copy(std::shared_ptr<SocketChannelImp> src) {
        // 除了manager其它都要进行深拷贝
        auto ret = std::make_shared<UDPSocketChannelImp>();
        ret->setAddress(this->getAddress());
        ret->setManager(this->getManager());
        return ret;
    }

    /************SocketManager***********
     *
 ____             _        _   __  __
/ ___|  ___   ___| | _____| |_|  \/  | __ _ _ __   __ _  __ _  ___ _ __
\___ \ / _ \ / __| |/ / _ \ __| |\/| |/ _` | '_ \ / _` |/ _` |/ _ \ '__|
 ___) | (_) | (__|   <  __/ |_| |  | | (_| | | | | (_| | (_| |  __/ |
|____/ \___/ \___|_|\_\___|\__|_|  |_|\__,_|_| |_|\__,_|\__, |\___|_|
                                                        |___/
     *
     *
    */

    SocketManager::SocketManager(SocketManager&& other) {
        CPS::SocketFd temp;
        temp.store(other.fd.load(),std::memory_order_release);
        other.fd.store(this->fd.load(),std::memory_order_release);
        this->fd.store(temp.load(),std::memory_order_release);
    }

    void SocketManager::close(){
        CPS::SocketLibraryManager::manager().close(this->fd);
    }

    SocketManager::~SocketManager() {
        try {
            this->close();
        } 
        catch (std::exception& e) {
            std::cerr << e.what() << std::endl;
            // 程序中断
            std::abort();
        }
    }
}