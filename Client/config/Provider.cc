//
// Created by kiosk on 2020/1/31.
//
#include "Provider.h"
Provider::Provider(){
    auto fp = fopen("/etc/init.json", "rb");
    auto Buffer = std::make_unique<char[]>(4096);
    rapidjson::FileReadStream Read(fp, Buffer.get(), 4096);
    fclose(fp);
    rapidjson::Document doc;
    doc.ParseStream(Read);
    m_port = doc["Server"]["port"].GetInt();
    m_ip = doc["Server"]["ip"].GetString();
    m_path = doc["Client"]["path"].GetString();
    m_mac = doc["Client"]["mac"].GetString();
}