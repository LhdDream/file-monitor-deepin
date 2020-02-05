//
// Created by kiosk on 2020/1/31.
//

#ifndef FILE_MONITOR_DEEPIN_CLIENT_H
#define FILE_MONITOR_DEEPIN_CLIENT_H
//与Server通信和Epoll
#include "Epoll.h"
#include "Socket.h"
#include "../hook/Hook.h"
#include "../../config/Provider.h"

class Client{
public:
    explicit Client():m_client(),
    m_epoll(),m_Hook(m_epoll){
        m_Hook.SetCallback([&]() -> bool{return m_client.StateConnect();});
        m_Hook.SetFd([&]() {return m_client.GetFd();});
    }
    void Run();
private:
    Socket m_client;
    Epoll m_epoll;
    Hook m_Hook;
};

#endif //FILE_MONITOR_DEEPIN_CLIENT_H
