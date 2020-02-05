//
// Created by kiosk on 2020/2/1.
//

#ifndef FILE_MONITOR_DEEPIN_THREADPOOL_H
#define FILE_MONITOR_DEEPIN_THREADPOOL_H

#include <mutex>
#include <condition_variable>
#include <queue>
#include <thread>
#include <functional>


class thread_pool {
public:

    explicit thread_pool():m_data_(std::make_shared<m_data>()){
        for(int  i = 0 ; i < std::thread::hardware_concurrency();i++){
            std::thread([data = m_data_] {
                std::unique_lock<std::mutex> lk(data->m_mutex);
                while(true) {
                    if (!data->m_queue.empty()) {
                        auto current = std::move(data->m_queue.front());
                        data->m_queue.pop();
                        lk.unlock();
                        current();
                        lk.lock();
                    } else if (data->m_stop) {
                        break;
                    } else {
                        data->m_cond.wait(lk);
                    }
                }
            }).detach();
        }
    }


    ~thread_pool(){
        m_data_->m_stop = true;
        m_data_->m_cond.notify_all();
    }
    template <typename  T>
    void AddTask(T && f)
    {
        m_data_->m_queue.emplace(std::forward<T>(f));
        m_data_->m_cond.notify_one();
    }

private:
    struct m_data {
       std::mutex m_mutex;
       std::condition_variable m_cond;
       bool m_stop = false;
       std::queue<std::function<void()>> m_queue;
    };
    std::shared_ptr<m_data> m_data_;
};

#endif //FILE_MONITOR_DEEPIN_THREADPOOL_H
