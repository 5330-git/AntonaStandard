#ifndef CPS_SOCKET_HPP
#define CPS_SOCKET_HPP
#include <algorithm>
#include <CPS/Socket.h>
namespace std{
    template<>
    void swap<AntonaStandard::CPS::SocketAddress>(AntonaStandard::CPS::SocketAddress& lhs,AntonaStandard::CPS::SocketAddress& rhs){
        lhs.swap(rhs);
    }
}
#endif
