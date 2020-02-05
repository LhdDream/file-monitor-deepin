//
// Created by kiosk on 2020/2/2.
//
#include "Client.h"
void Client::Run() {
    m_client.Createfd(Provider::Get().GetIp().c_str(),Provider::Get().GetPort());
    m_client.Connect();
    m_epoll.Add_Channel(m_client.GetFd(),Readable());
    m_Hook.Init(Provider::Get().GetPath());
   while(true) {
       int re = m_epoll.Wait();
       auto &&Result = m_epoll.GetEvent();
       for (int i = 0; i < re; i++) {
           if (Result[i].data.fd != m_client.GetFd()) {
               //监控事件逻辑
               m_Hook.RunOnce(Result[i].data.fd);
           } else {
               //服务器事件逻辑
               if(Result[i].events & ::EPOLLRDHUP)
               {
                   m_client.SetState(Socket::K_Break_Link);
                   m_client.Connect();
               }
               if(Result[i].events & Readable()){
                    File::RecvFile(Result[i].data.fd);
               }
           }
       }
   }
}
