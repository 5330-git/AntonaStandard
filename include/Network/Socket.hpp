#ifndef NETWORK_SOCKET_HPP
#define NETWORK_SOCKET_HPP
#include <algorithm>
#include <Network/Socket.h>
namespace std{
    template<>
    void swap<AntonaStandard::Network::SocketAddress>(AntonaStandard::Network::SocketAddress& lhs,AntonaStandard::Network::SocketAddress& rhs){
        lhs.swap(rhs);
    }
}
#endif