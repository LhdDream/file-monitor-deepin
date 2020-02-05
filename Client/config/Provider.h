//
// Created by kiosk on 2020/1/31.
//

#ifndef FILE_MONITOR_DEEPIN_PROVIDER_H
#define FILE_MONITOR_DEEPIN_PROVIDER_H
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <memory>
class Provider{
public:
    std::string GetPath() const {
        return m_path;
    }
    std::string GetIp() const {
        return m_ip;
    }
    size_t  GetPort() const {
        return m_port;
    }
    std::string GetMac() const {
        return m_mac;
    }

    static Provider & Get()  {
        static Provider cp;
        return cp;
    }
protected:
    Provider();
private:
    std::string m_path = ""; // client
    std::string m_ip = "127.0.0.1";
    std::string m_mac = "";
    size_t  m_port = 8080;
};

#endif //FILE_MONITOR_DEEPIN_PROVIDER_H
