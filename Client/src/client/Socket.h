//
// Created by kiosk on 2020/2/1.
//

#ifndef FILE_MONITOR_DEEPIN_SOCKET_H
#define FILE_MONITOR_DEEPIN_SOCKET_H

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstring>




//客户端的连接状态
class Socket{
public:
    enum State : uint8_t {
        K_DisConnect,
        K_Connected,
        K_Break_Link
    };
public:
    Socket() = default;
    Socket(int fd){
        m_client_fd = fd;
    }
    ~Socket() {
        close(m_client_fd);
    }
    void Createfd(const char *ip = "127.0.0.1", const int port = 8080){
        m_client_fd = socket(PF_INET, SOCK_STREAM  | SOCK_CLOEXEC, 0);
        bzero(&m_address, sizeof(struct sockaddr_in));
        m_address.sin_family = AF_INET;
        inet_pton(AF_INET,ip,&m_address.sin_addr);
        m_address.sin_port = htons(port);
    }

    int GetFd() const {
        return m_client_fd;
    }
    void Connect()  {
        auto ret =  ::connect(m_client_fd, reinterpret_cast<const struct sockaddr *> (&m_address), sizeof(m_address));
        if(ret == 0 ){
            m_Connect_State = K_Connected;
        }
    }
    bool StateConnect() const{
                return m_Connect_State == K_Connected;
    }
    void SetState(State e) {
        m_Connect_State = e;
    }

    int Send(const void *buffer, int length, int flags)const {
        return ::send(m_client_fd,buffer,length,flags);
    }
    int  Recv( void *buffer, int length, int flags) const{
        return ::recv(m_client_fd,buffer,length,flags);
    }
private:

    int m_client_fd = 0 ;
    struct sockaddr_in m_address{};
    State m_Connect_State  = K_DisConnect;// 连接状态
};
#endif //FILE_MONITOR_DEEPIN_SOCKET_H
