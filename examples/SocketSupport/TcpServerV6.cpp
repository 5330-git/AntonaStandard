#include "MultiPlatformSupport/SocketSupport.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include <iostream>
#include <random>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport;

int main(){
	cout<<"Lauching TCP V6 Socket Server..."<<endl;

	Socket server_socket = SocketCommunication::get().createSocket(SocketProtocol::ipv6,
	SocketType::Stream,11111,"0:0:0:0:0:0:0:1");							// 创建套接字
	try{
		SocketCommunication::get().bindSocket(server_socket);				// 绑定套接字
		// 设置监听
		SocketCommunication::get().listenSocket(server_socket);				// 监听套接字
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
	// 等待连接
	cout<<"Waiting for Clients connection..."<<endl;
	Socket client_socket = SocketCommunication::get().acceptSocket(server_socket);		// 等待连接
	cout<<"Connected with client: "<<client_socket.getAddress()->getIp()<<":"<<client_socket.getAddress()->getPort()<<endl;
	
	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	string message;
	while(true){
		buf.clear();
		size_t len = SocketCommunication::get().receive(client_socket,buf);

		if(len == 0){
			cout<<"Break the connection"<<endl;
			break;
		}
		// 按行读取
		std::getline(stream,message);
		cout<<"From Client: "<<endl<<"  "<<message<<endl;
		buf.clear();
		stream<<"hello from server : "<<dis(gen)<<endl;
		
		SocketCommunication::get().send(client_socket,buf);
	}
	// 手动关闭
	SocketCommunication::get().closeSocket(server_socket);
	SocketCommunication::get().closeSocket(client_socket);
	#ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif

	return 0;
}




