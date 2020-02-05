//
// Created by kiosk on 2020/2/3.
//

#ifndef FILE_SERVER_USER_H
#define FILE_SERVER_USER_H
#include <sys/types.h>
#include <sys/socket.h>
#include <climits>
#include <cstring>
#include <functional>
#include <sys/stat.h>
#include <sys/types.h>
#include "../net/Epoll.h"
#include "../log/log.h"

class User{
public:
    struct data{
        char  mac[32]; //mac地址
        char  path[PATH_MAX]; // 文件路径
        char  content[8192];
        bool  sign ; // 事件标志 open  false   or close true
        int   n; // 第几块内容
    };

public:
    User() :m_st(Readable()),Data(std::make_unique<struct data>()){

    }
     bool Read(int fd);
     bool Write(int fd);

     void SetType(EpollEventType m){
         m_st = m;
     }
     EpollEventType GetType() const {
         return m_st;
     }

private:
    EpollEventType m_st = Readable();
    std::unique_ptr<struct data> Data ;
};
#endif //FILE_SERVER_USER_H
