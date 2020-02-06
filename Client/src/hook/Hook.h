//
// Created by kiosk on 2020/1/31.
//

#ifndef FILE_MONITOR_DEEPIN_HOOK_H
#define FILE_MONITOR_DEEPIN_HOOK_H

#include <exception>
#include <dirent.h>
#include <fcntl.h>
#include <functional>
#include <map>
#include <unordered_map>
#include <set>
#include "Currency.h"
#include "../../config/Provider.h"
#include "../client/Epoll.h"
#include "../client/File.h"
#include "../client/threadpool.h"

class Hook{
    using ProcessLineState = std::function<bool() >;
    using ServerFd = std::function<int ()>;
public:
   explicit  Hook( Epoll& epoll) ;
   ~Hook();
   bool Init( std::string &&FilePath); // 添加到fanotify中

   void RunOnce(int fd);

   void SetCallback(ProcessLineState && callback){
       m_callback = std::move(callback);
   }
   void SetFd(ServerFd && callback){
       m_serverfd = std::move(callback);
   }

private:
    //发送标志位


    Epoll& m_epoll;
    std::unique_ptr<thread_pool> m_poll;
    std::vector<int> m_table;
    std::unordered_map<std::string,std::unique_ptr<File> > m_count;   //所有文件的引用计数
    ProcessLineState m_callback; //回调事件
    ServerFd  m_serverfd; // 回调
private:
    int AlloworDisAllow(int fd , int flag, int filefd);
    auto Getcount(const std::string & namefile) -> decltype(m_count[namefile]);
};
#endif //FILE_MONITOR_DEEPIN_HOOK_H
