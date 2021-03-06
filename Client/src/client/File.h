//
// Created by kiosk on 2020/1/31.
//

#ifndef FILE_MONITOR_DEEPIN_FILE_H
#define FILE_MONITOR_DEEPIN_FILE_H
//对于文件内容和路径的读取
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cassert>
#include <climits>
#include <cstring>
#include <memory>
#include <string>

#include "../../config/Provider.h"
#include "Socket.h"
#include "threadpool.h"
struct data {
    char mac[32];         // mac地址
    char path[PATH_MAX];  // 文件路径
    char content[8192];
    bool sign;    // 事件标志 open  false   or close true
    long long n;  // 目前文件长度
};

constexpr static char m_wirte[] = "It is a secret";
class File {
    friend class Hook;

public:
    explicit File() : m_count(0), m_Backup(false) {
    }
    static std::string ReadPath(int fd) {
        char buffer[PATH_MAX + 1];
        sprintf(buffer, "/proc/self/fd/%d", fd);
        size_t len = readlink(buffer, buffer, PATH_MAX);
        buffer[len] = '\0';
        return std::string(buffer);
    }
    void ModifyFile(int filefd);
    void ReadySend(int fd, const std::unique_ptr<thread_pool>& pool, std::string& path);
    //获取文件内容
    void SendClose(int client, const std::string& name);
    static void RecvFile(Socket&& client);

private:
    int m_count;    // 标志完全关闭
    bool m_Backup;  // 备份成功标志位文件只备份一次
};
#endif  // FILE_MONITOR_DEEPIN_FILE_H
