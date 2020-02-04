//
// Created by kiosk on 2020/2/3.
//

#ifndef FILE_SERVER_EPOLL_H
#define FILE_SERVER_EPOLL_H
#include <sys/epoll.h>
#include <fcntl.h>
#include <memory>
#include <sys/epoll.h>
#include <array>
#include <unistd.h>

enum EpollEventType{
    KReadble = ::EPOLLIN,
    KWriteable = ::EPOLLOUT,
    KLISTEN = ::EPOLLEXCLUSIVE,
    KClose = ::EPOLLRDHUP
};

constexpr EpollEventType Readable() { return EpollEventType( EpollEventType::KReadble | EpollEventType::KClose ); }

constexpr EpollEventType Writeable() { return EpollEventType(EpollEventType::KWriteable| EpollEventType::KClose ); }


class Epoll {
public:
    explicit Epoll() : m_epollfd( ::epoll_create1(EPOLL_CLOEXEC)){

    }

    ~Epoll() {
        close(m_epollfd);
    }

    int Add_Channel(int fd , EpollEventType && type) const  {
        struct epoll_event it {} ;
        it.data.fd = fd ;
        it.events = type;
        return epoll_ctl(m_epollfd, EPOLL_CTL_ADD,fd , &it);
    }

    int Remove_Channel(int fd , EpollEventType  &&type) const {
        struct epoll_event it{} ;
        it.data.fd = fd ;
        it.events = type;
        return epoll_ctl(m_epollfd, EPOLL_CTL_DEL, fd, &it);
    }

    int Update_Channel(int fd , EpollEventType  &&type) const {
        struct epoll_event it {};
        it.data.fd = fd ;
        it.events = type;
        return epoll_ctl(m_epollfd, EPOLL_CTL_MOD, fd, &it);
    }

    int Wait() {
        int user_number = epoll_wait(m_epollfd, m_event.data(), m_event.size(), -1);
        return user_number;
    }
    auto GetEvent()  {
        return m_event;
    }
private:
    std::array<struct epoll_event,1024> m_event;
    int m_epollfd;
};


#endif //FILE_SERVER_EPOLL_H
