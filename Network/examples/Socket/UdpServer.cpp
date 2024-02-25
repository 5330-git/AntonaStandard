#include <Network/Socket.h>
#include <iostream>
using namespace AntonaStandard;
/*
本程序 sock1 占用127.0.0.1:9000 sock2 127.0.0.1:9001
目标通信程序 sock1 占用127.0.0.1:10000 sock2 占用127.0.0.1:10001

127.0.0.1:10000 ---> 127.0.0.1:9000
127.0.0.1:9000 ---> 127.0.0.1:10001
127.0.0.1:10001 ---> 127.0.0.1:9001
127.0.0.1:9001 ---> 127.0.0.1:10000

从而完成一个通信闭环,每个通道接收到套接字后会将自己的地址添加到接收到的地址之后然后回应回去
程序结束后打印每个套接字的收发缓冲

*/

int main(){
    // 绑定到本地任意地址的9000端口
    Network::UDPSocket sock9000(CPS::SocketAddress::anyAddress(9000,CPS::AddressFamily::ipv4));
    
    // 绑定到9001端口 
    Network::UDPSocket sock9001(CPS::SocketAddress::anyAddress(9001,CPS::AddressFamily::ipv4));

    // 启动套接字
    sock9000.launch();
    sock9001.launch();
    
    // 创建套接字通道
    Network::SocketChannel channel_9000_10000 = sock9000.getChannel();
    Network::SocketChannel channel_9000_10001 = sock9000.getChannel();
    Network::SocketChannel channel_9001_10001 = sock9001.getChannel();
    Network::SocketChannel channel_9001_10000 = sock9001.getChannel();
    
    // 对需要发送数据的通道设置目标地址
    channel_9000_10001.setAddress(CPS::SocketAddress::loopBackAddress(10001,CPS::AddressFamily::ipv4));
    channel_9001_10000.setAddress(CPS::SocketAddress::loopBackAddress(10000,CPS::AddressFamily::ipv4));

    // channel_9000_10000等待数据
    auto _9000_10000_read = channel_9000_10000.read();
    std::cout<<"sock9000 accepted "<<_9000_10000_read<<" Byte data from "<<channel_9000_10000.getAddress().getIP()<<":"<<channel_9000_10000.getAddress().getPort()<<" automatically"<<std::endl;
    std::string msg_9000_10000;
    std::getline(std::istream(&channel_9000_10000.getInBuffer()),msg_9000_10000);

    // channel_9000_10001发送数据
    std::ostream(&channel_9000_10001.getOutBuffer())<<msg_9000_10000<<"[127.0.0.1:9000] ";
    channel_9000_10001.write();

    // channel_9001_10001等待数据
    auto _9001_10001_read = channel_9001_10001.read();
    std::cout<<"sock9001 accepted "<<_9001_10001_read<<" Byte data from "<<channel_9001_10001.getAddress().getIP()<<":"<<channel_9001_10001.getAddress().getPort()<<" automatically"<<std::endl;
    std::string msg_9001_10001;
    std::getline(std::istream(&channel_9001_10001.getInBuffer()),msg_9001_10001);

    // channel_9001_10000发送数据
    std::ostream(&channel_9001_10000.getOutBuffer())<<msg_9001_10001<<"[127.0.0.1:9001] ";
    channel_9001_10000.write();

    // 打印最后一次收到的数据
    std::cout<<"Final: "<<std::endl;
    std::cout<<msg_9001_10001;

    // 关闭套接字
    sock9000.close();
    sock9001.close();
}