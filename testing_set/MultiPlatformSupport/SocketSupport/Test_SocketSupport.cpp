#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "MultiPlatformSupport/SocketSupport.h"
#include <iostream>
#include <random>
#include <thread>
#include <vector>

// 定义事件监听类，监听内存泄漏
class SocketAddressMemoryLeakChecker: public ::testing::EmptyTestEventListener{
public:
    long memory_usage_start_ = 0;
    long memory_usage_end_ = 0;
    virtual void OnTestStart(const ::testing::TestInfo& test_info){
        
        memory_usage_start_ = AntonaStandard::MultiPlatformSupport::SocketAddress::getAllocatedObjectCount();
        
    }
    virtual void OnTestEnd(const ::testing::TestInfo& test_info){
        memory_usage_end_ = AntonaStandard::MultiPlatformSupport::SocketAddress::getAllocatedObjectCount();
        EXPECT_EQ(memory_usage_start_, memory_usage_end_)<<"Memory Leak Detected! Defferences:"<<memory_usage_end_ - memory_usage_start_<<" Objcets have not been released";;
    }
};

std::random_device rd;
std::mt19937 gen(rd());
std::uniform_int_distribution<> dis(10000, 1000000);

// 简单读写
TEST(Test_SocketDataBuffer, Test_Buffer_SinpleIO){
    using AntonaStandard::MultiPlatformSupport::SocketDataBuffer;
    SocketDataBuffer buffer;
    std::iostream is(&buffer);
    std::vector<int> datas;
    int io_times = 10000;
    // 简单读写
    for(int i=0;i<io_times;++i){
        datas.push_back(dis(gen));
        is<<datas.back()<<" ";
    }

    // 检查内容是否正确
    for(int i=0;i<io_times;++i){
        int data;
        is>>data;
        EXPECT_EQ(data, datas[i]);
    }
}

TEST(Test_SocketDataBuffer, Test_Buffer_Write_Between){
    // 主要测试缓冲对象的 sendingPos 和 getSendableSize是否正常工作
    using AntonaStandard::MultiPlatformSupport::SocketDataBuffer;
    SocketDataBuffer buffer;
    std::iostream is(&buffer);
    // 写入数据，用于发送
    std::string data1 = "And all the men and women merely players\n";
    std::string data2 = "往来的男女只是过场的演员\n";
    std::string data3 = "They have their exits and their entrances\n他们有登场，也有退场\nAnd one man in his time plays many parts\n终其一生要扮演许多角色\n";
    auto data = data1 + data2 + data3;
    is<<data1<<data2<<data3;
    // 检查数据
    for(char* i = buffer.sendingPos();i<buffer.sendingPos()+buffer.getSendableSize();++i){
        EXPECT_EQ(*i, data[i-buffer.sendingPos()]);
    }
}

TEST(Test_SocketDataBuffer, Test_Buffer_Read_Between){
    // 主要测试缓冲对象的 recievingPos 和 getRecivableSize是否正常工作
    using AntonaStandard::MultiPlatformSupport::SocketDataBuffer;
    SocketDataBuffer buffer;
    std::iostream is(&buffer);
    // 写入数据，用于发送
    std::vector<std::string> datas;
    datas.push_back("And all the men and women merely players\n");
    datas.push_back("往来的男女只是过场的演员\n");
    datas.push_back("They have their exits and their entrances\n");
    datas.push_back("他们有登场，也有退场\n");
    datas.push_back("And one man in his time plays many parts\n");
    datas.push_back("终其一生要扮演许多角色\n");
    // 
    for(auto& data : datas){
        for(int i = 0;i<data.size();++i){
            if(i >= buffer.getReceivableSize()){
                // 扩展到原来的2倍
                buffer.resize(buffer.size()*2);
            }
            *(buffer.receivingPos()+i) = data[i];
        }
        // 移动写入位置
        buffer.movePutPos(data.size());
    }
    std::string recvData;

    for(auto& data: datas){
        recvData.clear();
        std::getline(is, recvData);
        recvData += "\n";
        EXPECT_EQ(recvData, data);
    }
    
}

// 由于本项目直接调用系统API，需要进行真实接口测试（本地回环测试）

// 定义数据集（在测试夹具中定义）
class DataFixture: public ::testing::Test{
public:
    std::vector<std::string> datas;
    virtual void SetUp()override{
        datas.push_back("And all the men and women merely players");
        datas.push_back("往来的男女只是过场的演员");
        datas.push_back("They have their exits and their entrances");
        datas.push_back("他们有登场，也有退场");
        datas.push_back("And one man in his time plays many parts");
        datas.push_back("终其一生要扮演许多角色");
        datas.push_back("But through most of it they are the same");
        datas.push_back("His acts being seven ages");
        datas.push_back("At first, the infant");
        datas.push_back("起初是婴儿");
        datas.push_back("Mewling and puking in the nurse's arms");
        datas.push_back("在护士怀里啼哭，呕吐");
        datas.push_back("Then the whining schoolboy, with his satchel");
        datas.push_back("然后是背着书包的学童");
        datas.push_back("And shining morning face, creeping like snail");
        datas.push_back("脸上闪烁着朝阳，蜗牛似的爬行");
        datas.push_back("Unwillingly to school");
        datas.push_back("不情愿地挪向学堂");
        datas.push_back("And then the lover");
        datas.push_back("然后坠入爱河");
        datas.push_back("Sighing like furnace, with a woeful ballad");
        datas.push_back("哀怨的叹着气");
        datas.push_back("Made to his mistress' eyebrow");
        datas.push_back("为爱人的眉眼赋诗作词");
        datas.push_back("Then a soldier");
        datas.push_back("然后是士兵");
        datas.push_back("Full of strange oaths and bearded like the pard");
        datas.push_back("念着古怪的誓言，蓄起厚重的胡须");
        datas.push_back("Jealous in honor, sudden and quick in quarrel");
        datas.push_back("争名斗誉，好勇斗狠");
        datas.push_back("Seeking the bubble reputation");
        datas.push_back("即使面对炮火");
        datas.push_back("Even in the cannon's mouth");
        datas.push_back("也要追求虚妄的名誉");
        datas.push_back("And then the justice");
        datas.push_back("然后是法官");
        datas.push_back("In fair round belly with good capon lined");
        datas.push_back("被酒肉养的大腹便便");
        datas.push_back("With eyes severe and beard of formal cut");
        datas.push_back("眼神凌厉，胡须整洁");
        datas.push_back("Full of wise saws and modern instances");
        datas.push_back("满嘴格言和老生常谈");
        datas.push_back("And so he plays his part");
        datas.push_back("他便这样演下去");
        datas.push_back("The sixth age shifts into the lean and slippered pantaloon");
        datas.push_back("第六幕他成了消瘦又拖拉的老朽");
        datas.push_back("With spectacles on nose and pouch on side");
        datas.push_back("驾着眼镜，挂着钱袋");
        datas.push_back("His youthful hose, well saved");
        datas.push_back("穿了一辈子的长袜虽然完好");
        datas.push_back("A world too wide for his shrunk shank, and his big manly voice");
        datas.push_back("却套不上瘦削的小腿，浑厚的声线");
        datas.push_back("Turning again toward childish treble, pipes");
        datas.push_back("也变成了细锐的童声");
        datas.push_back("And whistles in his sound");
        datas.push_back("像吹着风笛和哨子");
        datas.push_back("Last scene of all, that ends this strange eventful history");
        datas.push_back("最后一幕，终了这曲折离奇的一生");
        datas.push_back("Is second childishness and mere oblivion");
        datas.push_back("重回孩提，忘却此生");
        datas.push_back("Sans teeth, sans eyes, sans taste, sans everything");
        datas.push_back("没有牙齿，没有眼睛，没有味觉，什么都没有");
    }
};

TEST_F(DataFixture, TCP_V4){
    // TCP V4 套接字
    std::function<void()> client_task = [&](){
        using namespace AntonaStandard::MultiPlatformSupport;
        Socket server_socket = SocketCommunication::get().createSocket(
            SocketProtocol::ipv4,
            SocketType::Stream,
            9999,
            "127.0.0.1");
        // 链接服务器套接字
        SocketCommunication::get().connectSocket(server_socket);
        // 发送datas 中的数据
            // 设置缓冲区
        SocketDataBuffer buffer;
        std::iostream buffer_stream(&buffer);
        buffer.resize(128);
        for(auto& data: datas){
            buffer.clear();
            buffer_stream << data <<std::endl;
            // 发送
            SocketCommunication::get().send(server_socket,buffer);
            // 清空缓冲，接收服务端数据
            buffer.clear();
            buffer.resize(128);
            SocketCommunication::get().receive(server_socket,buffer);
            std::string data_recv;
            std::getline(buffer_stream,data_recv);
            EXPECT_EQ(data, data_recv);
            // 休眠100ms 等待传输
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    std::function<void()> server_task = [&](){
        using namespace AntonaStandard::MultiPlatformSupport;
        Socket server_socket = SocketCommunication::get().createSocket(
            SocketProtocol::ipv4,
            SocketType::Stream,
            9999,
            "127.0.0.1");
        // 绑定服务器套接字
        SocketCommunication::get().bindSocket(server_socket);
        // 监听
        SocketCommunication::get().listenSocket(server_socket);

        Socket client_socket = SocketCommunication::get().acceptSocket(server_socket);

        EXPECT_EQ(client_socket.getAddress()->getIp(),"127.0.0.1");
        EXPECT_EQ(server_socket.getAddress()->getPort(),9999);
        EXPECT_EQ(client_socket.getAddress()->getProtocol(),SocketProtocol::ipv4);
        // 发送datas 中的数据
            // 设置缓冲区
        SocketDataBuffer buffer;
        std::iostream buffer_stream(&buffer);
        buffer.resize(128);
        for(auto& data: datas){
            // 清空缓冲，接收服务端数据
            buffer.clear();
            SocketCommunication::get().receive(client_socket,buffer);
            std::string data_recv;
            std::getline(buffer_stream,data_recv);
            EXPECT_EQ(data, data_recv);
            // 清空缓冲，接收服务端数据
            buffer.clear();
            // 休眠100ms 等待传输
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            buffer_stream << data <<std::endl;
            // 发送
            SocketCommunication::get().send(client_socket,buffer);
            
        }
    };
    std::thread server_th(server_task);
    // 保证服务端先运行，休眠一秒
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread client_th(client_task);
    
    
    client_th.join();
    server_th.join();
}

TEST_F(DataFixture, TCP_V6){
    // TCP V4 套接字
    std::function<void()> client_task = [&](){
        using namespace AntonaStandard::MultiPlatformSupport;
        Socket server_socket = SocketCommunication::get().createSocket(
            SocketProtocol::ipv6,
            SocketType::Stream,
            10000,
            "::1");
        // 链接服务器套接字
        SocketCommunication::get().connectSocket(server_socket);
        // 发送datas 中的数据
            // 设置缓冲区
        SocketDataBuffer buffer;
        std::iostream buffer_stream(&buffer);
        buffer.resize(128);
        for(auto& data: datas){
            buffer.clear();
            buffer_stream << data <<std::endl;
            // 发送
            SocketCommunication::get().send(server_socket,buffer);
            // 清空缓冲，接收服务端数据
            buffer.clear();
            buffer.resize(128);
            SocketCommunication::get().receive(server_socket,buffer);
            std::string data_recv;
            std::getline(buffer_stream,data_recv);
            EXPECT_EQ(data, data_recv);
            // 休眠100ms 等待传输
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    };
    std::function<void()> server_task = [&](){
        using namespace AntonaStandard::MultiPlatformSupport;
        Socket server_socket = SocketCommunication::get().createSocket(
            SocketProtocol::ipv6,
            SocketType::Stream,
            10000,
            "::1");
        // 绑定服务器套接字
        SocketCommunication::get().bindSocket(server_socket);
        // 监听
        SocketCommunication::get().listenSocket(server_socket);

        Socket client_socket = SocketCommunication::get().acceptSocket(server_socket);

        EXPECT_EQ(client_socket.getAddress()->getIp(),"::1");
        EXPECT_EQ(server_socket.getAddress()->getPort(),10000);
        EXPECT_EQ(client_socket.getAddress()->getProtocol(),SocketProtocol::ipv6);
        // 发送datas 中的数据
            // 设置缓冲区
        SocketDataBuffer buffer;
        std::iostream buffer_stream(&buffer);
        buffer.resize(128);
        for(auto& data: datas){
            // 清空缓冲，接收服务端数据
            buffer.clear();
            SocketCommunication::get().receive(client_socket,buffer);
            std::string data_recv;
            std::getline(buffer_stream,data_recv);
            EXPECT_EQ(data, data_recv);
            // 清空缓冲，接收服务端数据
            buffer.clear();
            // 休眠100ms 等待传输
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            buffer_stream << data <<std::endl;
            // 发送
            SocketCommunication::get().send(client_socket,buffer);
            
        }
    };
    std::thread server_th(server_task);
    // 保证服务端先运行，休眠一秒
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    std::thread client_th(client_task);
    
    client_th.join();
    server_th.join();
}