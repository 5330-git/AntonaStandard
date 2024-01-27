#include <Network/Socket.h>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;
using namespace AntonaStandard;
int main(){
    Network::Socket::SocketLibraryManager::launch();
    Network::Tcp::LocalSocket local_socket(Network::SocketAddress::loopBackAddress(8888,Network::IPType::ipv4));
    
    local_socket.bind();
    local_socket.listen();
    auto address = local_socket.accept();
    Network::SocketDataBuffer inbuf;
    Network::SocketDataBuffer outbuf;
    inbuf.resize(1024);
    outbuf.resize(1024);
    iostream instream(&inbuf);
    iostream outstream(&outbuf);
    // 优先进行发送
    for(int i = 0;i < 10;i++){
        cout<<"Receive and send by 'receive' and 'send' :"<<endl;

        // 发送
        outbuf.clear();
        outstream<<"Hello,World! : "<<i+100<<endl;
        local_socket.send(outbuf);

        // 接收
        inbuf.clear();
        local_socket.receive(inbuf);
        string str;
        std::getline(instream,str);
        cout<<"Receive from "<<address.getIP()<<":"<<address.getPort()<<" : "<<str<<endl;

        cout<<"Receive and send by 'receiveFrom' and 'sendTo' :"<<endl;
// 发送
        outbuf.clear();
        outstream<<"Hello,World! : "<<i+200<<endl;
        local_socket.sendTo(address,outbuf);
// 接收        
        inbuf.clear();
        local_socket.receiveFrom(address,inbuf);
        std::getline(instream,str);
        cout<<"Receive from "<<address.getIP()<<":"<<address.getPort()<<" : "<<str<<endl;
        cout<<endl;

        std::this_thread::sleep_for(chrono::seconds(1));

    }
    return 0;
}