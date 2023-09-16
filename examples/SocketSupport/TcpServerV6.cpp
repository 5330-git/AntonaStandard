#include "MultiPlatformSupport/SocketSupport.h"
#include <iostream>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport;

int main(){
	SocketCommunication socketCommunication;			// 创建套接字通信
	Socket server_socket = socketCommunication.createSocket(SocketProtocol::ipv6,
	SocketType::Stream,11111,"0:0:0:0:0:0:0:1");							// 创建套接字
	// char ip[32];
	// inet_ntop(AF_INET,&addr_in.sin_addr,ip,32);						// 获取IP地址
	// cout<<ip;
	try{
		socketCommunication.bindSocket(server_socket);				// 绑定套接字
	// 设置监听
	socketCommunication.listenSocket(server_socket);				// 监听套接字
	}
	catch(std::runtime_error& e){
		cout<<e.what()<<endl;
		return 0;
	}
	// 绑定套接字
	// 等待连接
	Socket client_socket = socketCommunication.acceptSocket(server_socket);		// 等待连接
	// 接收数据
	cout<<"开始收集数据"<<endl;
	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	system("chcp 65001");
	while(true){

		size_t len = socketCommunication.receive(client_socket,buf);

		if(len == 0){
			cout<<"断开连接"<<endl;
			break;
		}
		cout<<"来自客户端 "<<client_socket.getAddress()->getIp()<<" : ";
        cout<<client_socket.getAddress()->getPort()<<" 的数据："<<buf.str()<<endl;
		buf.resize(128);
		stream<<flush;
		stream<<"hello from server";
		socketCommunication.send(client_socket,buf);
	}
	
	return 0;
}




