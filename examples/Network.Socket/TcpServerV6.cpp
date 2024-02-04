#include <iostream>
#include <Network/Socket.h>
using namespace std;
using namespace AntonaStandard;
int main(){
    // 创建监听套接字,监听9000端口
    Network::TCPListenSocket lissock(CPS::SocketAddress::anyAddress(9000,CPS::AddressFamily::ipv6));
    // 启动监听套接字
    lissock.launch();

    // 接收请求
    Network::TCPSocket sock = lissock.accept();

    // 获取套接字通道用于通信
    Network::SocketChannel channel = sock.getChannel();

    // 查看连接的主机地址
    cout<<channel.getAddress().getIP()<<":"<<channel.getAddress().getPort()<<endl;

    // 获取输入流和输出流
    std::istream ins(&channel.getInBuffer());
    std::ostream outs(&channel.getOutBuffer());

    // 收发
	while(true){
		// 接收
		channel.getInBuffer().clear();
		auto recv_size = channel.read();
		if(recv_size == 0){
			break;
		}
		std::string role;
		ins>>role;
		int num;
		ins>>num;
		cout<<"Received data size: "<<recv_size<<" byte"<<endl;
		std::cout<<"Received: '"<<role<<" "<<num<<"'"<<std::endl;

		outs<<"Server: "<<num+3<<std::endl;
		std::cout<<"Sent: "<<num+3<<std::endl;
		// 发送
        size_t sendable_size = channel.getOutBuffer().getSendableSize();
		size_t send_size = channel.write();
		std::cout<<"Sent "<<send_size<<"/"<<sendable_size<<std::endl;
	}
	lissock.close();
	sock.close();
}