#include "MultiPlatformSupport/SocketSupport.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport; 

int main(){
	
	Socket server_socket = SocketCommunication::get().createSocket(SocketProtocol::ipv4,
	SocketType::Stream,9999,"127.0.0.1");							// 创建套接字
	// 连接服务器

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(100, 1000);
	cout<<"Connect to the sever..."<<endl;
	SocketCommunication::get().connectSocket(server_socket);
	cout<<"Successfully connect to the server !"<<endl;
	
	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	string message;
	for(int i = 0;i<20;++i){
		// 发送20次数据
		buf.clear();
		stream<<"The "<<i<<" times sending : "<<"hello from client! "<<dis(gen) <<endl;
		SocketCommunication::get().send(server_socket,buf);
		buf.clear();
		size_t len = SocketCommunication::get().receive(server_socket,buf);
		std::getline(stream,message);
		cout<<"Message size: "<<message.size()<<endl;
		cout << "Message from server: "<<server_socket.getAddress()->getIp()<<":"<<server_socket.getAddress()->getPort() <<endl<<"   "<<message<<endl;

		// 暂停一下等待
		this_thread::sleep_for(std::chrono::seconds(1));
	}
	// SocketCommunication::get().closeSocket(server_socket);			// 可以选择手动关闭套接字
	#ifdef AntonaStandard_PLATFORM_WINDOWS
        system("pause");
    #endif

	return 0;
}