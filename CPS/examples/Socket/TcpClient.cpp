#include <CPS/Socket.h>
#include <iostream>
#include <thread>
#include <chrono>
using namespace AntonaStandard;
int main(){
	CPS::SocketFd communication_fd = CPS::SocketLibraryManager::manager().socket(CPS::AddressFamily::ipv4, CPS::SocketType::Stream, CPS::SocketProtocol::TCP);
	CPS::SocketAddress remote_addr(CPS::AddressFamily::ipv4, "127.0.0.1", 8080);
	CPS::SocketLibraryManager::manager().connect(communication_fd, remote_addr);

	// 定义缓冲
	CPS::SocketDataBuffer inbuf;
	CPS::SocketDataBuffer outbuf;
	inbuf.resize(1024);
	outbuf.resize(1024);
	std::iostream ins(&inbuf);
	std::iostream outs(&outbuf);
	// 发送数字，
	for(int i = 0;i<10;++i){
		outs<<"Client: "<<i<<std::endl;
		std::cout<<"Sent: "<<i<<std::endl;
		size_t sendable_size = outbuf.getSendableSize();
		size_t send_size = CPS::SocketLibraryManager::manager().send(communication_fd, outbuf);
		std::cout<<"Sent: "<<send_size<<"/"<<sendable_size<<std::endl;
		outbuf.clear();

		inbuf.clear();
		// 接收
		size_t recv_size = CPS::SocketLibraryManager::manager().receive(communication_fd, inbuf);

		

		std::string role;
		ins>>role;
		int num;
		ins>>num;
		std::cout<<"Received: '"<<role<<" "<<num<<'\''<<std::endl;

	}
	// 关闭套接字
	CPS::SocketLibraryManager::manager().close(communication_fd);
	return 0;

}