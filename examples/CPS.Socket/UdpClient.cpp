#include <CPS/Socket.h>
#include <iostream>
using namespace AntonaStandard;
int main(){
	CPS::SocketFd communication_fd = CPS::SocketLibraryManager::manager().socket(CPS::AddressFamily::ipv4,CPS::SocketType::Datagram,CPS::SocketProtocol::UDP);
	CPS::SocketAddress remote_addr(CPS::AddressFamily::ipv4,"127.0.0.1",1234);

	// 直接通信
	CPS::SocketDataBuffer inbuf;
	CPS::SocketDataBuffer outbuf;
	inbuf.resize(1024);
	inbuf.resize(1024);
	std::iostream ins(&inbuf);
	std::iostream outs(&outbuf);

	for(int i=0;i<10;i++){
		outs<<"Client: "<<i<<std::endl;
		CPS::SocketLibraryManager::manager().sendTo(communication_fd,outbuf,remote_addr);
		std::cout<<"Send: 'Client: "<<i<<"'"<<std::endl;
		outbuf.clear();

		inbuf.clear();
		CPS::SocketLibraryManager::manager().receiveFrom(communication_fd,inbuf,remote_addr);
		std::string role;
		ins>>role;
		int num = 0;
		ins>>num;
		std::cout<<"Recveived: '"<<remote_addr.getIP()<<":"<<remote_addr.getPort()<<" "<<role<<" "<<num<<"'"<<std::endl;

	}	
	CPS::SocketLibraryManager::manager().close(communication_fd);
}