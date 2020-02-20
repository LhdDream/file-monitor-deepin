//
// Created by kiosk on 2020/2/5.
//

#ifndef FILE_SERVER_THREADPOOL_H
#define FILE_SERVER_THREADPOOL_H

#include <thread>
#include <utility>
#include <vector>

#include "../Server/Server.h"

thread_local Server m_Server;

class Proxy {
public:
    explicit Proxy() {
        thcont_.reserve(3);
        for (int i = 0; i < 3; i++) {
            thcont_.emplace_back([]() { m_Server.Run(); });
        }
        m_Server.Run();
    }

    int wait() {
        for (auto& i : thcont_) {
            i.join();
        }

        return 0;
    }

private:
    std::vector<std::thread> thcont_;
};

#endif  // FILE_SERVER_THREADPOOL_H
