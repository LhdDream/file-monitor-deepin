//
// Created by kiosk on 2020/2/3.
//

#ifndef FILE_SERVER_SERVER_H
#define FILE_SERVER_SERVER_H

#include <unordered_map>

#include "../core/User.h"
#include "../net/Epoll.h"
#include "../net/Socket.h"
class Server {
public:
    Server() : m_Epoll(), m_Socket(){};
    void Run();

private:
    Epoll m_Epoll;
    Socket m_Socket;
    std::unordered_map<int, std::unique_ptr<User>> m_table;
};
#endif  // FILE_SERVER_SERVER_H
