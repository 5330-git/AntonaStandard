#include "MultiPlatformSupport/SocketSupport.h"
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport; 

int main(){
	SocketCommunication socketCommunication;			// 创建套接字通信
	Socket server_socket = socketCommunication.createSocket(SocketProtocol::ipv6,
	SocketType::Stream,11111,"0:0:0:0:0:0:0:1");							// 创建套接字
	// 连接服务器
	system("chcp 65001");
	cout<<"准备链接服务器"<<endl;
	socketCommunication.connectSocket(server_socket);
	cout<<"连接服务器成功"<<endl;
	
	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	
	for(int i = 0;i<20;++i){
		// 发送20次数据
		buf.resize(128);				// 重新设置缓冲区大小
		stream<<flush;					// 清空缓冲
		stream<<"hello from client!"<<"第"<<i<<"次发送";
		socketCommunication.send(server_socket,buf);
		size_t len = socketCommunication.receive(server_socket,buf);
		cout<<"来自服务端 "<<server_socket.getAddress()->getIp()<<" : ";
        cout<<server_socket.getAddress()->getPort()<<" 的数据："<<buf.str()<<endl;
		// 暂停一下等待
		this_thread::sleep_for(std::chrono::seconds(1));
	}
	socketCommunication.closeSocket(server_socket);			// 关闭套接字
	
	return 0;
}