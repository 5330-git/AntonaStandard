/**
 * @file Socket.hpp
 * @author Anton (yunye_helloworld@qq.com)
 * @brief 特化swap
 * @version 1.0.0
 * @date 2024-03-07
 * 
 * @copyright Copyright (c) 2024
 * 
 */
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
