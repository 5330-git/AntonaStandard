#include "MultiPlatformSupport/SocketSupport.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include <iostream>
#include <random>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport;

int main(){
	cout<<"Lauching UDP V6 Socket Server..."<<endl;
	
	Socket server_socket = SocketCommunication::get().createSocket(SocketProtocol::ipv6,
	SocketType::Dgram,11111,"0:0:0:0:0:0:0:1");							// 创建套接字
	try{
		SocketCommunication::get().bindSocket(server_socket);				// 绑定套接字
		// 无需监听
	}
	catch(std::runtime_error& e){
		cout<<e.what()<<endl;
		return 0;
	}
	// 设置随机数，用于发给客户端
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(100, 1000);
	// 绑定套接字
	// Udp 无需等待连接
	std::shared_ptr<SocketAddress> client_address_ptr;				// 用于获取接收到的客户端地址数据

	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	string message;
	while(true){
		buf.clear();
		size_t len = SocketCommunication::get().receiveFrom(server_socket,client_address_ptr,buf);
		// 按行读取
		std::getline(stream,message);
		cout<<"From Client "<<client_address_ptr->getIp()<<" : "<<client_address_ptr->getPort()<<" : "<<endl<<"  "<<message<<endl;
		buf.clear();
		stream<<"hello from server : "<<dis(gen)<<endl;
		
		SocketCommunication::get().sendTo(server_socket,client_address_ptr,buf);
	}
	// 手动关闭
	SocketCommunication::get().closeSocket(server_socket);
	#ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif
	return 0;
}




