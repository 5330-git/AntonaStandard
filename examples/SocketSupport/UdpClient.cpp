#include "MultiPlatformSupport/SocketSupport.h"
#include "MultiPlatformSupport/MultiPlatformMacro.h"
#include <iostream>
#include <chrono>
#include <thread>
#include <random>
using namespace std;
using namespace AntonaStandard::MultiPlatformSupport; 

int main(){
	cout<<"Lauching UDP V4 Socket Client..."<<endl;
	Socket server_socket = SocketCommunication::get().createSocket(SocketProtocol::ipv4,
	SocketType::Dgram,9999,"127.0.0.1");							// 创建套接字

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(100, 1000);
	
	
	SocketDataBuffer buf;
	iostream stream(&buf);
	buf.resize(128);
	string message;

	auto server_addr_ptr = server_socket.getAddress();           // 对于UDP 来说这里只期占位作用
	// 直接发送
	for(int i = 0;i<20;++i){
		// 发送20次数据
		buf.clear();
		stream<<"The "<<i<<" times sending : "<<"hello from client! "<<dis(gen) <<endl;
		SocketCommunication::get().sendTo(server_socket,server_addr_ptr,buf);
		buf.clear();
		size_t len = SocketCommunication::get().receiveFrom(server_socket,server_addr_ptr,buf);
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