#ifndef CPS_SOCKET_H
#define CPS_SOCKET_H

/**
 * @file Socket.h
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 套接字的跨平台低层次封装
 * @details 目前分别封装了 Windows 平台和 Linux 平台的套接字库，对外提供一致API接口 
 * @version 1.0.0 
 * @date 2024-03-03
 * 
 * @copyright Copyright (c) 2024
 * 
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

/*
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
    /**
     * @brief 封装地址协议类型
     */
    enum AddressFamily{
        ipv4 = AF_INET,           ///< ipv4 地址协议
        ipv6 = AF_INET6,          ///< ipv6 地址协议
        invalid = ~0,             ///< 无效地址协议
    };

    /**
     * @brief 封装套接字类型
     * @details 
     *      目前版本只封装了流式协议和报文式协议。一般情况下流式套接字使用的默认协议是TCP协议
     *      而报文套接字使用的默认协议是UDP协议。有关套接字协议详见`AntonaStandard::CPS::SocketProtocol`
     * @see
     *      AntonaStandard::CPS::SocketProtocol
     */
    enum SocketType{
        Stream = SOCK_STREAM,       ///< 流式套接字类型
        Datagram = SOCK_DGRAM,      ///< 报文套接字类型
        InvalidType = ~0,           ///< 无效套接字类型
    };

    /**
     * @brief 套接字协议类型
     * @details
     *      目前版本只封装了TCP和UDP协议，它们分别是 **流式套接字类型** 和 **报文式套接字类型** 的默认协议
     *      如果函数指定了套接字类型并且你明确需要使用默认类型，那么套接字协议类型一般不需要显式指定
     * @see
     *      AntonaStandard::CPS::SocketType
     */
    enum SocketProtocol{            
        Default = 0,                 ///< 使用默认协议
        TCP = IPPROTO_TCP,           ///< 使用TCP协议，在TCP协议头中这个值对应6
        UDP = IPPROTO_UDP,           ///< 使用UDP协议，在UDP协议投中这个值对应17 
        InvalidProtocol = ~0,        ///< 无效的协议
    };

    /**
     * @brief 套接字选项
     * @details 
     *      套接字选项用于指定是否允许套接字进行端口复用和地址复用。详情可见getSocketOption 函数和 setSocketOption 函数
     * @see 
     *      void AntonaStandard::CPS::SocketLibraryManager::setSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,const void* optval,socklen_t optlen)
     *      void AntonaStandard::CPS::SocketLibraryManager::getSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,void* accept_optval,socklen_t* accept_optlen)
     */
    enum SocketOptions{            // 套接字选项
        // ReusePort = SO_REUSEPORT,     // 允许端口复用
        ReuseAddress = SO_REUSEADDR,        ///< 允许地址和端口复用
        // KeepAlive = SO_KEEPALIVE,       // 保持连接
        // NoDelay = TCP_NODELAY,         // 禁用Nagle算法
        /*其它选项后续用到再添加*/
        InvalidOption = ~0,                 ///< 无效的套接字选项
    };

    /**
     * @brief 套接字选项作用的层
     * @details 
     *      详细可见 getSocketOption 函数 和 setSocketOption 函数
     * @see
     *      void AntonaStandard::CPS::SocketLibraryManager::setSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,const void* optval,socklen_t optlen)
     *      void AntonaStandard::CPS::SocketLibraryManager::getSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,void* accept_optval,socklen_t* accept_optlen)
     * 
     */
    enum SocketLevel{
        Socket_level = SOL_SOCKET,          ///< 套接字层
        IP_level = IPPROTO_IP,              ///< IP层
        TCP_level = IPPROTO_TCP,            ///< TCP层
        InvalidLevel = ~0,                  ///< 无效层
    };

    /**
     * @brief 选择如何关闭套接字
     * @details 
     *      除了可以调用close 函数关闭套接字，还可以调用 shutdown 函数关闭套接字。你可以选择只关闭读通道或写通道，
     *      或者将读写通道都关闭。详情可见 SocketLibraryManager 类的 close 和 shutdown 函数 
     * @see
     *      void AntonaStandard::CPS::SocketLibraryManager::close(SocketFd&)
     *      void AntonaStandard::CPS::SocketLibraryManager::shutdown(SocketFd,ShutdownOptions)
     */
    enum ShutdownOptions{
        #ifdef AntonaStandard_PLATFORM_WINDOWS
            Read = SD_RECEIVE,          // 关闭读通道
            Write = SD_SEND,             // 关闭写通道
            Both = SD_BOTH,              // 关闭读写通道
        #elif AntonaStandard_PLATFORM_LINUX
            Read = SHUT_RD,              // 关闭读通道
            Write = SHUT_WR,              // 关闭写通道
            Both = SHUT_RDWR,               // 关闭读写通道
        #else
            Read = ~0,                   ///< 关闭读通道
            Write = ~0,                  ///< 关闭读写通道
            Both = ~0,                   ///< 关闭读写通道
        #endif
        InvalidShutdownOption = ~0,
    };


    /**
     * @brief 套接字文件描述符的封装，本质是原子类，用于解决可能的条件竞争问题
     * @details
     *      **Windows 平台** ：Windows 平台下 SocketFd 本质是 `std::antomic<SOCKET>`
     *      **Linux 平台** ：Linux 平台下 SocketFd 本质是 `std::atomic<int>`
     */
    using SocketFd = 
    #ifdef AntonaStandard_PLATFORM_WINDOWS
        std::atomic<SOCKET>;
    #elif AntonaStandard_PLATFORM_LINUX
        std::atomic<int>;
    #else
        std::atomic<~>;
        #error "Unsupported platform"
    #endif
    
    /**
     * @brief 套接字地址封装类 SocketAddress 的实现接口类
     * @details 
     *      Windows 和 Linux 平台的套接字通信库中是通过一个名为 `sockaddr_t` 的结构体指针来传递存储IP地址的结构体的
     *      `sockaddr_in` 和 `sockaddr_in6` 结构体分别用于存储IPV4地址和IPV6。`sockaddr_t` 相当于一个接口类，
     *      而 `sockaddr_in` 和 `sockaddr_in6` 相当于实现类，当传递它们的指针时需要强制转化成 `sockaddr_t` 的指针
     *      同时还需要传入结构体的大小，对应的系统API内部会根据结构体大小，判断具体的类型是 `sockaddr_in` 还是 `sockaddr_in6`
     *      这相当于用C语言实现了简单但巧妙地多态。另外本类中没有纯虚函数，也就是说本类不是抽象类，是可以实例化的。原因是
     *      SocketAddress 类中有一个imp 成员共享指针变量指向具体的 SocketAddressImp 派生类。SocketAddress 所有的对外接口
     *      都需要通过这个指针访问具体的 SocketAddressImp 派生类的接口，如果创建SocketAddress 时指定的ip地址无效或者其它问题导致
     *      没有成功创建 SocketAddressImp 派生类的实例，那么需要为它设置一个默认指针，而设置成空指针会增大判断成本以及会造成访问空指针的
     *      异常。因此没有成功创建派生类的实例时会创建 SocketAddressImp 的实例，它的接口会返回一些无效值，这样在使用它的其它函数中会对这些无效值
     *      进行处理。
     * @see
     *      AntonaStandard::CPS::SocketAddress
     */
    class SocketAddressImp{
    protected:
        /**
         * @brief 存储系统API需要使用的地址结构体指针
         * @details
         *      这里使用空类型指针存储真实的地址结构体，因为任何类型的指针都可以自动转化为空类型指针。这样可以根据具体的实现类
         *      中的构造函数为 addr_in 变量赋值 `sockaddr_in` 或 `sockaddr_in6` 结构体的指针。另外为了避免可能产生的内存泄露
         *      问题，这里使用共享指针来管理内存释放。
         */
        std::shared_ptr<void> addr_in;      
    public:
        SocketAddressImp()=default;         ///< 使用默认生成的默认构造函数
        /**
         * @brief 获取 addr_in 成员变量
         * @return std::shared_ptr<void> 
         */
        inline std::shared_ptr<void> getAddrIn()const{
            return addr_in;
        }
        /**
         * @brief 获取 addr_in 成员指向的具体结构体的大小
         * @return size_t 
         */
        virtual size_t getAddrInSize()const;
        /**
         * @brief 设置地址和端口 
         * @details
         *      该函数会调用系统API `inet_addr` 在addr_in 指向的地址结构体中写入地址，调用
         *      系统API `htons` 向addr_in 指向的地址结构体中写入端口号。另外该函数不会检查
         *      地址字符串和端口是否合法。对地址字符串和端口的检查会在使用该类实例的时候进行
         * @param ip ip 地址字符串
         * @param port 端口号
         */
        virtual void setAddr(const char* ip,
                                unsigned short port);
        /**
         * @brief 获取地址字符串
         * @details
         *      该函数会调用系统API `inet_ntop` 从addr_in 指向的地址结构体中解析地址字符串
         * @return std::string 
         */
        virtual std::string getIP()const;
        /**
         * @brief 获取端口
         * @details
         *      该函数会调用系统API `ntohs` 从addr_in 指向的地址结构体中解析端口号
         * @return unsigned short 
         */
        virtual unsigned short getPort()const;
        /**
         * @brief 获取该实例的地址簇协议 
         * @return AddressFamily 
         * @see AntonaStandard::CPS::AddressFamily
         */
        virtual AddressFamily getAddressFamily()const;
        virtual ~SocketAddressImp()=default;        ///< 生成默认析构函数，并声明为虚函数
        /**
         * @brief 深拷贝当前实例，并以共享指针的形式返回
         * @return std::shared_ptr<SocketAddressImp> 
         */
        virtual std::shared_ptr<SocketAddressImp> copy();
        /**
         * @brief 根据地址结构体来构造SocketAddressImp派生类
         * @details
         *      SocketAddressImp 及其派生类的工厂函数，会根据 `addr_len` 的长度判断传入的 `addr` 指向的是
         *      `sockaddr_in` 还是 `sockaddr_in6`。内部通过 `memcpy` 函数进行基于位的拷贝
         * @param addr          地址结构体的指针
         * @param addr_len      地址结构体的大小
         * @return std::shared_ptr<SocketAddressImp> 
         *  @retval
         *      std::shared_ptr<SocketAddressImp>       如果 `addr_len` 的长度与 `sockaddr_in`或 `sockaddr_in6` 都不相等。或者addr为空指针
         *  @retval
         *      std::shared_ptr<SocketAddressV4Imp>     如果 `addr_len` 的长度与 `sockaddr_in` 相等
         *  @retval
         *      std::shared_ptr<SocketAddressV6Imp>     如果 `addr_len` 的长度与 `sockaddr_in6` 相等
         */
        static std::shared_ptr<SocketAddressImp> create(const sockaddr* addr,socklen_t addr_len);
    };

    /**
     * @brief 套接字地址封装类 SocketAddress 的实现类，派生自 SocketAddresssImp，存储 ipv4 地址
     * @details
     *      该类派生自 SocketAddressImp 存储和维护一个指向 `sockaddr_in` 的共享指针
     */
    class SocketAddressV4Imp:public SocketAddressImp{
    public:
        /**
         * @brief 默认构造函数
         * @details
         *      其内部会调用 std::make_shared<sockaddr_in> 构建一个可以存储ipv4地址的结构体，并将其地址赋值给 addr_in
         */
        SocketAddressV4Imp();                  
        SocketAddressV4Imp(SocketAddressV4Imp& addr);           ///< 拷贝构造函数
        SocketAddressV4Imp(SocketAddressV4Imp&& addr);          ///< 移动构造函数
        ~SocketAddressV4Imp()=default;                          
        virtual size_t getAddrInSize()const override;           
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        /**
         * @throw
         *      std::runtime_error 如果从地址结构体中解析地址字符串时失败了会抛出该异常
         */
        virtual std::string getIP()const override;
        virtual unsigned short getPort()const override;
        virtual AddressFamily getAddressFamily()const override;
        /**
         * @return std::shared_ptr<SocketAddressImp> 
         *  @retval std::shared_ptr<SocketAddressV4Imp> 重写后实际返回的
         */
        virtual std::shared_ptr<SocketAddressImp> copy()override;
    };

    /**
     * @brief 套接字地址封装类 SocketAddress 的实现类，派生自 SocketAddresssImp，存储 ipv6 地址
     * @details
     *      该类派生自 SocketAddressImp 存储和维护一个指向 `sockaddr_in6` 的共享指针
     */
    class SocketAddressV6Imp:public SocketAddressImp{
    public:
        SocketAddressV6Imp();                
        SocketAddressV6Imp(SocketAddressV6Imp& addr);          ///< 拷贝构造函数 
        SocketAddressV6Imp(SocketAddressV6Imp&& addr);         ///< 移动构造函数
        ~SocketAddressV6Imp()=default;
        virtual size_t getAddrInSize()const override;
        virtual void setAddr(const char* ip,
                                unsigned short port)override;
        /**
         * @throw
         *      std::runtime_error 如果从地址结构体中解析地址字符串时失败了会抛出该异常
         */
        virtual std::string getIP()const override;
        virtual unsigned short getPort()const override;
        virtual AddressFamily getAddressFamily()const override;
        /**
         * @return std::shared_ptr<SocketAddressImp> 
         *  @retval std::shared_ptr<SocketAddressV6Imp> 重写后实际返回的
         */
        virtual std::shared_ptr<SocketAddressImp> copy()override;
    };

    /**
     * @brief 套接字地址类，通过 Pimpl 策略隐藏了具体的实现细节
     * @details 
     *      为了向上提供简单的功能，这里采用 Pimpl 策略隐藏具体的实现细节。所有的功能委托给 `SocketAddresImp` 及其派生类实现
     */
    class SocketAddress{
    private:
        std::shared_ptr<SocketAddressImp> imp;      ///< 指向实现类的指针
    public: 
        /**
         * @brief 根据地址簇协议、地址字符串以及端口构造实例
         * @param ip_type 地址簇协议 详见 AntonaStandard::CPS::AddressFamily
         * @param ip 
         * @param port 
         * 
         * @details
         *      该函数不会检查ip_type 、ip以及 port 的合法性
         */
        SocketAddress(AddressFamily ip_type,std::string ip,unsigned short port);
        SocketAddress(const SocketAddress&);            ///< 拷贝构造函数
        void swap(SocketAddress&);                      ///< 与参数所指对象交换imp
        const SocketAddress& operator=(const SocketAddress&);
        SocketAddress(SocketAddress&&);                 ///< 移动构造函数
        const SocketAddress& operator=(SocketAddress&&);
        /**
         * @brief 默认构造函数，由于未指定地址簇。默认为imp构造一个SocketAddressImp 实例
         */
        SocketAddress():imp(std::make_shared<SocketAddressImp>()){};
        ~SocketAddress()=default;
        /**
         * @brief 获取指向地址结构体的指针
         * @details
         *      具体功能委托给 SocketAddressImp::getAddrIn() 实现
         * @return const std::shared_ptr<void> 
         */
        inline const std::shared_ptr<void> getAddrIn()const{
            return this->imp->getAddrIn();
        }
        /**
         * @brief 获取地址结构体的大小
         * @details 
         *      具体功能委托给 SocketAddressImp::getAddrInSize() 实现
         * @return socklen_t 
         */
        inline socklen_t getAddrInSize()const{
            return this->imp->getAddrInSize();
        }
        /**
         * @brief 获取地址簇协议
         * @details
         *      具体功能委托给 SocketAddressImp::getAddressFamily() 实现
         * @return AddressFamily 
         */
        inline AddressFamily getAddressFamily()const{
            return this->imp->getAddressFamily();
        }
        /**
         * @brief 获取IP地址字符串
         * @details
         *      具体功能委托给 SocketAdddress::getAddressFamily() 实现
         * @return std::string 
         */
        inline std::string getIP()const{
            return this->imp->getIP();
        }
        /**
         * @brief 获取端口号
         * @details
         *      具体功能委托给 SocketAddress::getPort() 实现
         * @return unsigned short 
         */
        inline unsigned short getPort()const{
            return this->imp->getPort();
        }
        /**
         * @brief 设置实现类
         * 
         * @param imp 
         */
        inline void setImp(std::shared_ptr<SocketAddressImp> imp){
            this->imp = imp;
        }
        /**
         * @brief 获取实现类的指针
         * 
         * @return const std::shared_ptr<SocketAddressImp> 
         */
        inline const std::shared_ptr<SocketAddressImp> getImp()const{
            return this->imp;
        }
    public:
        /**
         * @brief 获取本地包含本地回环地址的 SocketAddress 实例
         * @param port 
         * @param ip_type 
         * @return SocketAddress 
         */
        static SocketAddress loopBackAddress(unsigned short port,AddressFamily ip_type=AddressFamily::ipv4);
        /**
         * @brief 获取一个空地址，本质是一个实现类实例是 SocketAddressImp 的 SocketAddress 实例
         * @return SocketAddress 
         */
        static SocketAddress emptyAddress();
        /**
         * @brief 任意地址，相当于 "0.0.0.0" 或 "::/0"
         * @param port 
         * @param ip_type 
         * @return SocketAddress 
         */
        static SocketAddress anyAddress(unsigned short port,AddressFamily ip_type=AddressFamily::ipv4);
    };
    
    /**
     * @brief 缓冲套接字通信过程中的数据
     * @details
     *      派生自 `std::streambuf` 这意味着你可以通过 `std::istream` 或者 `std::ostream` 像使用 `std::cin`
     *      和 `std::cout` 那样使用它
     */
    class SocketDataBuffer:public std::streambuf{
        // 使用string作为底层的缓冲容器
    private:
        /**
         * @brief 底层的缓冲容器
         * @details 
         *      套接字通信过程中的数据会被写入进去。使用 `std::string` 的原因是便于扩容
         */
        std::string buffer;
    protected:
        /**
         * @brief 处理向缓冲区写数据时向上溢出的问题
         * @details 
         *      `std::streambuf` 类通过三个指针来管理写入缓冲区：
         *      - pbase 指向写入缓冲区的下限
         *      - pptr  指向写入点
         *      - epptr 指向写入缓冲区的上限
         *      如果 `pptr == epptr` 则会触发该函数
         * 
         * @param c  在未溢出的情况下字符会写入到 pptr 的位置，发生溢出时这个还未写入的字符会随参数 c 传入该函数
         * @return std::streambuf::int_type 
         *  @retval 若 c==EOF ，则返回异于 EOF 的某值。否则，成功时返回 (unsigned char)(c) ，失败时返回 EOF 。
         * 
         */
        std::streambuf::int_type virtual overflow(std::streambuf::int_type c)override;
        /**
         * @brief 处理从缓冲区读数据时向下溢出的问题
         * @details
         *      `std::streambuf` 类通过三个指针来管理读取缓冲区
         *      - eback 指向读取缓冲区的下限
         *      - gptr  指向读取位点
         *      - egptr 指向读取缓冲区的上限
         * 
         * @return int_type 
         *  @retval 成功时为获取区中下个字符 (unsigned char)(*gptr()) ，失败时为 EOF 。
         */
        virtual int_type underflow()override;
        /**
         * @brief 处理将字符放回缓冲区时失败的情况
         * @details
         *      这有可能是放入的字符和之前读取的字符不同，或者写缓冲区空间不足
         * @param c 
         * @return std::streambuf::int_type 
         */
        std::streambuf::int_type virtual pbackfail(std::streambuf::int_type c)override;
        // // 遇到std::flush 和 std::endl时回刷新
        // int virtual sync()override;  // 不需要刷新，应当允许endl进行换行
    public:
        SocketDataBuffer();
        /**
         * @brief 获取接收数据的起始位点
         * @details 即写入位点，receive 函数会将接收到的数据从写入位点开始写入到缓冲区
         * @return char* 
         */
        inline char* receivingPos()const{
            // 获取发送的数据的起始位点
            return this->pptr();
        }
        /**
         * @brief 获取发送数据的起始位点
         * @details
         *      需要将整个写缓冲区的内容发送出去，因为用户操作标准流对象写入数据时操作的实际上是写缓冲
         *      而发送的过程是将用户写入的数据发送出去。
         * @return char* 
         */
        inline char* sendingPos()const{
            return this->pbase();
        }
        /**
         * @brief 获取可用于接收数据的剩余空间大小
         * @details
         *      即写缓冲区的上限减去写入点之间的
         * @return size_t 
         */
        inline size_t getReceivableSize()const{
            return this->epptr()-this->pptr();
        }
        /**
         * @brief 获取可发送数据大小
         * @details 
         *      即写缓冲区写入点减去写缓冲区下限
         *       
         * @return size_t 
         */
        inline size_t getSendableSize()const{
            return this->pptr()-this->pbase();
        }
        /**
         * @brief 返回整个缓冲区的内容
         * 
         * @return const std::string& 
         */
        inline const std::string& str()const{
            return this->buffer;
        }
        /**
         * @brief 对缓冲区进行扩容
         * @details
         *      扩容后的大小不会小于已经写入的缓冲区大小
         * 
         * @param size 
         */
        inline void resize(size_t size){
            
            auto pptr_pos = std::min(size_t(this->pptr()-this->pbase()),size);
            ++size;             // epptr() 的位置是不可放和不可读的 
            auto gptr_pos = std::min(size_t(this->gptr()-this->eback()),pptr_pos);      // 防止溢出

            this->buffer.resize(std::max(size_t(1),size));          // 要为epptr留出位置
            this->setp(&(this->buffer.front()),&(this->buffer.back()));
            this->pbump(pptr_pos);
            this->setg(this->pbase(),this->pbase()+gptr_pos,this->pptr());
        }
        /**
         * @brief 清空缓冲区
         * @details
         *      会重置写缓冲区和读缓冲区的指针
         */
        inline void clear(){
            // 对于放置区来说，需要重置放置位置
            // 将输入缓冲区的三个指针和输出缓冲区的三个指针重定向
                // 写入缓冲区的指针pbase,pptr,epptr
            this->setp(&(buffer.front()),&(buffer.back()));
                // 输出，读取缓冲区的指针eback,gptr,egptr
                    // 即将可读的内容清空
            this->setg(&(buffer.front()),&(buffer.front()),this->pptr());
        }
        /**
         * @brief 返回缓冲的总体大小
         * @return size_t 
         */
        inline size_t size()const{
            return this->buffer.size();
        }
        /**
         * @brief 移动写入位点
         * @details
         *      通过调用 std::streambuf::pbump 实现
         * 
         * @param size 
         */
        inline void movePutPos(size_t size){
            this->pbump(size);
        }
    };
    /**
     * @brief 套接字库管理器
     * @details
     *      提供套接字文件描述符创建、绑定地址、设置监听、设置连接、接收发送数据的接口
     *      Windows 平台下使用套接字库前需要调用 WSAtartup 函数加载套接字库，而套接字库使用完毕
     *      需要调用WSACleanup 卸载套接字库。而Linux 平台不需要。为了保证对外的API一致，这里统一使用
     *      SocketLibraryManager 和RAII策略来管理套接字库的加载和卸载。同时为了让用户调用套接字通信
     *      API 之前套接字库一定加载，所有的套接字通信 API 都被声明为 SocketLibraryManager 的成员函数
     *      并且 SocketLibraryManager 的实例只能通过静态函数接口 manager 获取。 
     * 
     */
    class SocketLibraryManager{
    private:
        /**
         * @brief 默认构造函数，声明为私有，Windows 平台下会调用WSAStartup
         */
        SocketLibraryManager();
    public:
        /// @brief 禁用拷贝构造函数 
        SocketLibraryManager(const SocketLibraryManager&)=delete;
        /// @brief 禁用拷贝赋值
        SocketLibraryManager& operator=(const SocketLibraryManager&)=delete;
        /// @brief 禁用构造函数
        SocketLibraryManager(SocketLibraryManager&&)=delete;
        /// @brief 禁用移动赋值
        SocketLibraryManager& operator=(SocketLibraryManager&&)=delete;
        /**
         * @brief 析构函数，Windows 平台下会调用WSACleanup
         */
        ~SocketLibraryManager();
        /**
         * @brief 获取 SocketLibraryManager 实例
         * @details 
         *      内部会实例化一个SocketLibrary 的局部静态变量，并返回它的引用。这样实现了单例模式，保证
         *      SocketLibrary 只会被实例化一次
         * @return SocketLibraryManager& 
         */
        static SocketLibraryManager& manager();

        /**
         * @brief 创建套接字文件描述符
         * @details
         *      该函数一般用于生成用于监听的TCP套接字文件描述符（服务端），或者用于通信的TCP套接字文件描述符（客户端）
         *      ，或者生成用于通信的UPD套接字文件描述符。默认生成支持ipv4的TCP套接字文件描述符
         * @param af_type    选择套接字文件描述符的地址簇协议，详见 AntonaStandard::CPS::AddressFamily
         * @param type       选择套接字文件描述符的类型，详见 AntonaStandard::CPS::SocketType
         * @param protocol   选择套接字文件描述符的协议，详见 AntonaStandard::CPS::SocketProtocol 
         * @return SocketFd 
         * @throw 
         *      std::runtime_error 创建套接字失败可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的手册跟踪异常
         */
        SocketFd socket(AddressFamily af_type=AddressFamily::ipv4,SocketType type=SocketType::Stream, SocketProtocol protocol=SocketProtocol::Default);
        /**
         * @brief 将文件描述符绑定到指定的地址
         * @details
         *      将文件描述符绑定到指定的地址后，该文件描述符才能进行监听。TCP 通信会有
         *      专门的监听套接字监听客户端的连接，而UDP 通信是无连接的，它只有用于通信的套接字，这个套接字
         *      兼顾通信和监听其它端是否发送来了数据
         * @param listen_fd 如果是TCP通信接收的是用于监听的文件描述符，如果是UDP通信接收的是用于通信的文件描述符
         * @param local_addr 绑定到本地的地址，并且只能绑定到本地地址
         * @throw
         *      std::runtime_error 绑定套接字失败可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        void bind(const SocketFd& listen_fd,const SocketAddress& local_addr);
        /**
         * @brief 对套接字设置监听
         * @warning 注意：只能用于TCP的监听套接字
         * @param listen_fd 用于监听的套接字文件描述符，由 socket 函数创建
         * @param backlog   设置最大监听的连接数
         * @throw
         *      std::runtime_error 监听套接字失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         * @see 
         *      SocketFd socket(AddressFamily af_type=AddressFamily::ipv4,SocketType type=SocketType::Stream, SocketProtocol protocol=SocketProtocol::Default)
         */
        void listen(const SocketFd& listen_fd,int backlog=SOMAXCONN);
        /**
         * @brief 连接到目标地址
         * @warning
         *      注意：只能用于TCP的通信套接字（客户端）
         * @param communication_fd 用于通信的套接字文件描述符，由 socket 函数创建
         * @param remote_addr      需要连接的远端地址
         * @throw
         *      std::runtime_error 连接到服务器套接字失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         * @see
         *      SocketFd socket(AddressFamily af_type=AddressFamily::ipv4,SocketType type=SocketType::Stream, SocketProtocol protocol=SocketProtocol::Default)
         */
        void connect(const SocketFd& communication_fd,const SocketAddress& remote_addr);
        /**
         * @brief 通过监听套接字获取客户端连接，返回对应连接用于通信的套接字文件描述符
         * @warning
         *      返回的套接字文件描述符用于通信，不能传参给 bind、listen、connect、accept 函数
         * @param listen_fd     监听套接字的文件描述符
         * @param remote_addr   可以是空地址，用于接收连接的远端主机的地址 
         * @return SocketFd     用于通信的文件描述符
         * @throw 
         *      std::runtime_error 接受套接字连接失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        SocketFd accept(const SocketFd& listen_fd,SocketAddress& remote_addr);
        /**
         * @brief TCP 通信专用，用于接收远端发送来的数据
         * 
         * @param communication_fd  用于通信的套接字文件描述符
         * @param buffer            用于接收数据的SocketDataBuffer对象
         * @param flags             用于控制消息接收，该参数再目前的项目中暂且用不到，所以没有进行封装，一般情况下填0即可
         * @return size_t           接收到的数据长度，长度为 0 则对方断开连接
         * @throw
         *      std::runtime_error 接收数据失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        size_t receive(const SocketFd& communication_fd,SocketDataBuffer& buffer,int flags=0);
        /**
         * @brief TCP 通信专用，用于向通信套接字指向的对端发送数据
         * 
         * @param communication_fd 用于通信的套接字文件描述符
         * @param buffer           用于发送数据的SocketDataBuffer对象
         * @param flags            控制消息发送，一般情况下填0即可 
         * @return size_t          实际发送的数据长度
         * @throw
         *      std::runtime_error 发送数据失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        size_t send(const SocketFd& communication_fd,const SocketDataBuffer& buffer,int flags=0);
        /**
         * @brief UDP 通信专用，用于接收任意一端发送来的数据
         * 
         * @param communication_fd  用于通信的套接字文件描述符
         * @param buffer            用于接收数据的SocketDataBuffer对象
         * @param remote_addr       用于接收数据来源的地址
         * @param flags             控制消息接收，一般情况下填0即可
         * @return size_t           实际接收到的数据长度
         * @throw
         *      std::runtime_error 接收数据失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        size_t receiveFrom(const SocketFd& communication_fd,SocketDataBuffer& buffer,SocketAddress& remote_addr,int flags=0);
        /**
         * @brief UDP 通信专用，用于向目标地址发送数据
         * 
         * @param communication_fd  用于通信的套接字文件描述符
         * @param buffer            用于发送数据的SocketDataBuffer对象
         * @param remote_addr       目标地址
         * @param flags             控制消息发送，一般情况下填0即可
         * @return size_t           实际发送的数据长度
         * @throw
         *      std::runtime_error 发送数据失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        size_t sendTo(const SocketFd& communication_fd,const SocketDataBuffer& buffer,const SocketAddress& remote_addr,int flags=0);
        /**
         * @brief 检查一个文件描述符能否关闭
         * 
         * @param fd  需要检查的文件描述符
         * @return true 可以关闭
         * @return false 不能关闭，文件描述符无效
         */
        inline bool isClosable(SocketFd& fd){
            return fd!=INVALID_SOCKET;
        }
        /**
         * @brief 关闭套接字文件描述符
         * @details 
         *      当文件描述符被成功关闭时，它的值会被设置成 INVALID_SOCKET，设置的原子操作策略是写优先，这样可以保证
         *      在多线程的情况下不会出现悬空引用的问题
         * @param fd 
         * @throw
         *      std::runtime_error 关闭套接字失败时可能抛出该异常
         */
        void close(SocketFd& fd);
        /**
         * @brief 有选择地关闭套接字文件描述符
         * @details 
         *      用户可以选择只关闭写通道或读通道，设置的原子操作策略时写优先
         * @param fd        
         * @param how       详见AntonaStandard::CPS::ShutdownOptions
         * @throw
         *      std::runtime_error 关闭套接字失败时可能抛出该异常
         */
        void shutdown(SocketFd& fd,ShutdownOptions how);
        /**
         * @brief 设置套接字选项
         * @details
         *      通过该函数可以决定是否启用地址和端口复用，启用一个一个地址可以绑定多个套接字文件描述符，该函数
         *      是套接字库API setsockopt 的简单封装，参数基本一一对应，详细的参数说明可以查看setsockopt的文档 
         * @param fd 
         * @param level     设置选项的层，一般选择Socket_Level
         * @param option    选项
         * @param optval    选项的值
         * @param optlen    选项值的长度
         * @throw
         *      std::runtime_error 设置套接字选项失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        void setSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,const void* optval,socklen_t optlen);
        /**
         * @brief 获取套接字选项
         * @details
         *      通过该函数可以查看套接字选项的值。该函数时套接字库API getsockopt 的简单封装，参数基本一一对应，
         *      详细的参数说明可以查看 getsockopt
         * @param fd 
         * @param level 
         * @param option 
         * @param accept_optval 
         * @param accept_optlen 
         * @throw
         *      std::runtime_error 获取套接字选项失败时可能抛出该异常，异常消息中包含平台相关的错误码，可以查看对应平台的文档跟踪异常
         */
        void getSocketOption(const SocketFd& fd,SocketLevel level,SocketOptions option,void* accept_optval,socklen_t* accept_optlen);
    };
}

#endif
