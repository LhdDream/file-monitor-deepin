//
// Created by kiosk on 2020/2/3.
//
#include "Server.h"
void Server::Run() {
    m_Socket.Createfd();
    m_Socket.SetResueport();
    m_Socket.BindAddress();
    m_Socket.Listen();
    m_Epoll.Add_Channel(m_Socket.Fd(),Readable());
    while(true){
        auto re = m_Epoll.Wait();
        auto && event = m_Epoll.GetEvent();
        for(int i = 0 ; i < re ; i++){
                if(event[i].data.fd == m_Socket.Fd()){
                        auto userid = m_Socket.Accpet();
                        m_Epoll.Add_Channel(userid,Readable());
                        m_table.emplace(userid,std::make_unique<User>());
                }else{
                    if(event[i].events & KReadble){
                      auto re =   m_table[event[i].data.fd]->Read(event[i].data.fd);
                      if(re == false){
                          m_Epoll.Update_Channel(event[i].data.fd,Writeable());
                      }
                    }else if(event[i].events & KWriteable){
                        auto re = m_table[event[i].data.fd]->Write(event[i].data.fd);
                        if(re == true){
                            m_Epoll.Update_Channel(event[i].data.fd,Readable());
                        }
                    }
                    else if(event[i].events & KClose){
                        m_Epoll.Remove_Channel(event[i].data.fd,m_table[event[i].data.fd]->GetType());
                        m_table.erase(event[i].data.fd);
                        close(event[i].data.fd);
                    }
                }
        }
    }
}
