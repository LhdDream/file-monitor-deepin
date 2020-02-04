//
// Created by kiosk on 2020/2/3.
//
#include "User.h"
bool User::Read(int fd) {
    std::string realpath;
    auto re = recv(fd,Data.get(), sizeof(struct data),0);
    //建立目录等事件
    if(re > 0) {
        if (!Data->sign) {
            mkdir(Data->mac, S_IRWXU);
            for (auto &c :Data->path) {
                if (c == '/')
                    c = '@';
            }
            realpath = Data->mac;
            realpath += '/';
            realpath += Data->path;
            fd = open(realpath.c_str(), O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
            lseek(fd,  Data->n, SEEK_SET);
            int count = write(fd, Data->content, sizeof(Data->content));
            close(fd);
        } else {
            return false;
        }
    }
    return true;
}

bool User::Write(int fd) {
    //发送事件
    //获取文件大小,然后发送
    std::string filepath = Data->mac;
    for(auto &c :Data->path){
        if(c== '/')
            c = '@';
    }
    filepath += '/' ;
    filepath += Data->path;
    struct stat64 st{};
    auto it =  stat64(filepath.c_str(),&st);
    if(it < 0 ){
        printf("%s\n",strerror(errno));
        return false;
    }
    int Writefd = open(filepath.c_str(),O_WRONLY);
    int count = 0;
    bzero(Data->content, sizeof(Data->content));
    while((count += read(Writefd,Data->content, sizeof(Data->content))) != st.st_size){
       auto sendit =  send(fd,Data.get(), sizeof(struct data),0);
       if(sendit < 0){
           return false;
       }
        bzero(Data->content, sizeof(Data->content));
    }
    return true;
}