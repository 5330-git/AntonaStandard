#include <Network/Socket.h>
#include <iostream>
using namespace AntonaStandard;
/*
本程序 sock1 占用::1:10000 sock2 ::1:10001
目标通信程序 sock1 占用::1:9000 sock2 占用::1:9001

::1:10000 ---> ::1:9000
::1:9000 ---> ::1:10001
::1:10001 ---> ::1:9001
::1:9001 ---> ::1:10000

从而完成一个通信闭环,每个通道接收到套接字后会将自己的地址添加到接收到的地址之后然后回应回去
程序结束后打印每个套接字的收发缓冲

*/

int main(){
    // 绑定到本地任意地址的10000端口
    Network::UDPSocket sock10000(CPS::SocketAddress::anyAddress(10000,CPS::AddressFamily::ipv6));
    
    // 绑定到10001端口 
    Network::UDPSocket sock10001(CPS::SocketAddress::anyAddress(10001,CPS::AddressFamily::ipv6));

    // 启动套接字
    sock10000.launch();
    sock10001.launch();
    
    // 创建套接字通道,假设目标会从
    Network::SocketChannel channel_10000_9000 = sock10000.getChannel();
    Network::SocketChannel channel_10001_9000 = sock10001.getChannel();
    Network::SocketChannel channel_10001_9001 = sock10001.getChannel();
    Network::SocketChannel channel_10000_9001 = sock10000.getChannel();
    
    // 对需要发送数据的通道设置目标地址
    channel_10000_9000.setAddress(CPS::SocketAddress::loopBackAddress(9000,CPS::AddressFamily::ipv6));
    channel_10001_9001.setAddress(CPS::SocketAddress::loopBackAddress(9001,CPS::AddressFamily::ipv6));

    // 从sock10000的channel_10000_9000 通道开始发送数据
    std::ostream(&channel_10000_9000.getOutBuffer())<<"[::1:10000] ";
    channel_10000_9000.write();

    // channel_10001_9000 等待9000端口发送数据
    auto _10001_9000_read = channel_10001_9000.read();
    std::cout<<"sock10001 accepted "<<_10001_9000_read<<" Byte data from "<<channel_10001_9000.getAddress().getIP()<<":"<<channel_10001_9000.getAddress().getPort()<<" automatically"<<std::endl;
    std::string msg_10001_9000;
    std::getline(std::istream(&channel_10001_9000.getInBuffer()),msg_10001_9000);

    // channel_10001_9001 发送数据
    std::ostream(&channel_10001_9001.getOutBuffer())<<msg_10001_9000<<"[::1:10001] ";
    channel_10001_9001.write();

    // channel_10000_9001 等待9001数据
    auto _10000_9001_read =  channel_10000_9001.read();
    std::cout<<"sock10000 accepted "<<_10000_9001_read<<" Byte data from "<<channel_10000_9001.getAddress().getIP()<<":"<<channel_10000_9001.getAddress().getPort()<<" automatically"<<std::endl;

    std::string msg_10000_9001;
    std::getline(std::istream(&channel_10000_9001.getInBuffer()),msg_10000_9001);

    std::cout<<"Final:"<<std::endl;
    std::cout<<msg_10000_9001;

    // 关闭套接字
    sock10000.close();
    sock10001.close();
}