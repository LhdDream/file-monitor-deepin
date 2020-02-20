//
// Created by kiosk on 2020/2/3.
//

#include "Socket.h"

size_t Socket::BindAddress() {
    return ::bind(m_sockfd, (struct sockaddr*)&m_address, sizeof(struct sockaddr));
}

size_t Socket::Listen() {
    return ::listen(m_sockfd, 4096);  //默认backlog
}

void Socket::SetResueport() {
    int reuse = 1;
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEPORT, &reuse, sizeof(int));
    ::setsockopt(m_sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(int));
}

size_t Socket::Accpet() {
    struct sockaddr_in client {};
    socklen_t client_addrlength = sizeof(client);
    int fd = ::accept(m_sockfd, (struct sockaddr*)&client, &client_addrlength);
    return fd;
}
