//
// Created by kiosk on 2020/2/3.
//
#include "User.h"
bool User::Read(int fd) {
    std::string realpath;
    auto re = recv(fd, Data.get(), sizeof(struct data), MSG_WAITALL);
    //建立目录等事件
    if (re > 0) {
        mkdir(Data->mac, S_IRWXU);
        for (auto& c : Data->path) {
            if (c == '/')
                c = '@';
        }
        realpath = Data->mac;
        realpath += '/';
        realpath += Data->path;
        fd = open(realpath.c_str(), O_CREAT | O_WRONLY, 0666);
        if (fd < 0) {
            Logger().info() << "Open " << strerror(errno);
        }
        lseek(fd, Data->n, SEEK_SET);
        int count = write(fd, Data->content, strlen(Data->content));
        close(fd);
    } else if (re < 0) {
        Logger().info() << "Recv " << strerror(errno);
    }
    return !Data->sign;
}

bool User::Write(int fd) {
    //发送事件
    //获取文件大小,然后发送
    for (auto& c : Data->path) {
        if (c == '/')
            c = '@';
    }
    std::string filepath = Data->mac;
    filepath += '/';
    filepath += Data->path;
    struct stat64 st {};
    auto it = stat64(filepath.c_str(), &st);
    if (it < 0) {
        Logger().info() << "filepath ERROR " << strerror(errno);
        return true;
    }
    int Writefd = open(filepath.c_str(), O_RDONLY);
    if (Writefd < 0) {
        Logger().info() << "filepath ERROR " << strerror(errno);
        return true;
    }
    int count = 0;
    bzero(Data->content, sizeof(Data->content));
    auto len = (sizeof(Data->content) > st.st_size) ? st.st_size : sizeof(Data->content);
    while (count != st.st_size) {
        count += read(Writefd, Data->content, len);
        len = (st.st_size - count) > sizeof(Data->content) ? sizeof(Data->content) : (st.st_size - count);
        auto sendit = send(fd, Data.get(), sizeof(struct data), 0);
        if (sendit < 0) {
            Logger().info() << "Send ERROR " << strerror(errno);
            return false;
        }
        bzero(Data->content, sizeof(Data->content));
    }
    return true;
}