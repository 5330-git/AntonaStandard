#include <CPS/Socket.h>
#include <iostream>
using namespace AntonaStandard;
int main(){
	CPS::SocketFd communication_fd = CPS::SocketLibraryManager::manager().socket(CPS::AddressFamily::ipv4,CPS::SocketType::Datagram,CPS::SocketProtocol::UDP);
	CPS::SocketAddress local_addr = CPS::SocketAddress::loopBackAddress(1234,CPS::AddressFamily::ipv4);
	// 服务器需要绑定本地地址
	CPS::SocketLibraryManager::manager().bind(communication_fd,local_addr);
	CPS::SocketAddress remote_addr;
	// 直接通信
	CPS::SocketDataBuffer inbuf;
	CPS::SocketDataBuffer outbuf;
	inbuf.resize(1024);
	inbuf.resize(1024);
	std::iostream ins(&inbuf);
	std::iostream outs(&outbuf);

	for(int i=0;i<10;i++){
		
		inbuf.clear();
		CPS::SocketLibraryManager::manager().receiveFrom(communication_fd,inbuf,remote_addr);
		std::string role;
		ins>>role;
		int num = 0;
		ins>>num;
		std::cout<<"Recveived: '"<<remote_addr.getIP()<<":"<<remote_addr.getPort()<<" "<<role<<" "<<num<<"'"<<std::endl;
		
		outs<<"Server: "<<num+3<<std::endl;
		CPS::SocketLibraryManager::manager().sendTo(communication_fd,outbuf,remote_addr);
		std::cout<<"Send: 'Server: "<<num+3<<"'"<<std::endl;
		outbuf.clear();

	}	
	CPS::SocketLibraryManager::manager().close(communication_fd);

}