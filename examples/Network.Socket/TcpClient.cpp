#include <Network/Socket.h>
#include <iostream>
#include <chrono>
#include <thread>
using namespace AntonaStandard;
using namespace std;
int main(){
    Network::Socket::SocketLibraryManager::launch();

    Network::Tcp::RemoteSocket remote_socket(Network::SocketAddress(Network::IPType::ipv4,8888,"127.0.0.1"));
    // 连接
    remote_socket.connect();
    Network::SocketDataBuffer inbuf;
    Network::SocketDataBuffer outbuf;
    inbuf.resize(1024);
    outbuf.resize(1024);
    iostream instream(&inbuf);
    iostream outstream(&outbuf);
    

    // 优先进行发送
    for(int i = 0;i < 10;i++){
        cout<<"Receive and send by 'receive' and 'send' :"<<endl;

        // 接收
        inbuf.clear();
        auto received_size = remote_socket.receive(inbuf);
        string str;
        std::getline(instream,str);
        cout<<"Receive "<<received_size<<"Byte from "<<remote_socket.getAddress().getIP()<<":"<<remote_socket.getAddress().getPort()<<" : "<<str<<endl;

        // 发送
        outbuf.clear();
        outstream<<"Hello,World! : "<<i<<endl;
        auto sended_size = remote_socket.send(outbuf);
        cout<<"Send "<<sended_size<<"Byte to "<<remote_socket.getAddress().getIP()<<":"<<remote_socket.getAddress().getPort()<<endl;

        cout<<"Receive and send by 'receiveFrom' and 'sendTo' :"<<endl;
        Network::SocketAddress remote_address;
        // 接收
        inbuf.clear();
        received_size = remote_socket.receiveFrom(remote_address,inbuf);
        std::getline(instream,str);
        cout<<"Receive "<<received_size<<"Byte from "<<remote_address.getIP()<<":"<<remote_address.getPort()<<" : "<<str<<endl;
        // 发送
        outbuf.clear();
        outstream<<"Hello,World! : "<<i<<endl;
        sended_size = remote_socket.sendTo(remote_address,outbuf);
        cout<<"Send "<<sended_size<<"Byte to "<<remote_address.getIP()<<":"<<remote_address.getPort()<<endl;
        std::this_thread::sleep_for(chrono::seconds(1));
    }
    return 0;
}