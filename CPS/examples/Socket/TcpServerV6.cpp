#include <CPS/Socket.h>
#include <iostream>
#include <thread>
#include <chrono>
using namespace AntonaStandard;
int main(){
	std::cout<<"Server is running..."<<std::endl;
	CPS::SocketFd listen_fd = CPS::SocketLibraryManager::manager().socket(CPS::AddressFamily::ipv6, CPS::SocketType::Stream, CPS::SocketProtocol::TCP);
	CPS::SocketAddress local_addr(CPS::AddressFamily::ipv6, "::1", 8080);

	// bind
	CPS::SocketLibraryManager::manager().bind(listen_fd, local_addr);
	std::cout<<"Successfully binded to "<<local_addr.getIP()<<":"<<local_addr.getPort()<<std::endl;
	// listen
	CPS::SocketLibraryManager::manager().listen(listen_fd);
	std::cout<<"Successfully listen to "<<local_addr.getIP()<<":"<<local_addr.getPort()<<std::endl;

	CPS::SocketAddress remote_addr;
	// accept
	CPS::SocketFd communication_fd = CPS::SocketLibraryManager::manager().accept(listen_fd,remote_addr);
	std::cout<<"Successfully accepted from "<<remote_addr.getIP()<<":"<<remote_addr.getPort()<<std::endl;

	// 定义缓冲
	CPS::SocketDataBuffer inbuf;
	CPS::SocketDataBuffer outbuf;
	inbuf.resize(1024);
	outbuf.resize(1024);
	std::iostream ins(&inbuf);
	std::iostream outs(&outbuf);
	// 收发
	while(true){
		// 接收
		inbuf.clear();
		auto recv_size = CPS::SocketLibraryManager::manager().receive(communication_fd, inbuf);
		if(recv_size == 0){
			break;
		}
		std::string role;
		ins>>role;
		int num;
		ins>>num;
		std::cout<<"Received: '"<<role<<" "<<num<<"'"<<std::endl;

		outs<<"Server: "<<num+3<<std::endl;
		std::cout<<"Sent: "<<num+3<<std::endl;
		// 发送
		size_t sendable_size = outbuf.getSendableSize();
		size_t send_size = CPS::SocketLibraryManager::manager().send(communication_fd, outbuf);
		outbuf.clear();
		std::cout<<"Sent "<<send_size<<"/"<<sendable_size<<std::endl;
	}
	CPS::SocketLibraryManager::manager().close(communication_fd);
	CPS::SocketLibraryManager::manager().close(listen_fd);
	return 0;
}