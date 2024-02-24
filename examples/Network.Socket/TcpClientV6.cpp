#include <iostream>
#include <Network/Socket.h>
using namespace std;
using namespace AntonaStandard;
int main(){
    // 创建TCP通信套接字,目标 127.0.0.1:9000
    Network::TCPSocket sock(CPS::SocketAddress(CPS::AddressFamily::ipv6,"::1",9000));
    // 启动通信套接字
    sock.launch();

    // 获取套接字通道用于通信
    Network::SocketChannel channel = sock.getChannel();

    // 查看连接的主机地址
    cout<<channel.getAddress().getIP()<<":"<<channel.getAddress().getPort()<<endl;

    // 获取输入流和输出流
    std::istream ins(&channel.getInBuffer());
    std::ostream outs(&channel.getOutBuffer());

    // 收发
	for(int i = 0;i<4;++i){
		channel.getOutBuffer().clear();
		outs<<"Client: "<<i<<std::endl;
		std::cout<<"Sent: "<<i<<std::endl;
		size_t sendable_size = channel.getOutBuffer().getSendableSize();
		size_t send_size = channel.write();
		std::cout<<"Sent: "<<send_size<<"/"<<sendable_size<<std::endl;
		channel.getOutBuffer().clear();
		channel.getInBuffer().clear();
		// 接收
		size_t recv_size = channel.read();

		std::string role;
		ins>>role;
		int num;
		ins>>num;
		std::cout<<"Received: '"<<role<<" "<<num<<'\''<<std::endl;

	}
	sock.close();
}