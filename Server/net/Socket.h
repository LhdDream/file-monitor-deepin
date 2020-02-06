//
// Created by kiosk on 2020/2/3.
//

#ifndef FILE_SERVER_SOCKET_H
#define FILE_SERVER_SOCKET_H
#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>

class Socket // 使用RAII 重新封装Socket 对象
{
public:
     Socket() = default;

    ~Socket() { close(m_sockfd); }

    void Createfd(const char *ip = "127.0.0.1", const int port = 8080){
        m_sockfd = socket(PF_INET, SOCK_STREAM  | SOCK_CLOEXEC, 0);
        bzero(&m_address, sizeof(struct sockaddr_in));
        m_address.sin_family = AF_INET;
        m_address.sin_addr.s_addr = INADDR_ANY;
        m_address.sin_port = htons(port);
    }
    int Fd() const { return m_sockfd; }
    size_t BindAddress();

    size_t Listen();

    size_t Accpet();

    void SetResueport(); // 开启端口复用


private:
    size_t m_sockfd{}; //套接字
    struct sockaddr_in m_address{};
};


#endif //FILE_SERVER_SOCKET_H
