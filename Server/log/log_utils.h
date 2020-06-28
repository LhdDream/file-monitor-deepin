#ifndef _LOG_UTILS_H_
#define _LOG_UTILS_H_

#include <stdint.h>
#include <string>
#include <sstream>
#include <iomanip>

class LogUtils
{
public:
    // 二进制转为日志格式
    static std::string Bin2HexLog(const char* buff, int len)
    {
        std::ostringstream ss;

        for (uint32_t i = 0; i < len; i++)
        {
            if (i % 32 == 0)
            {
                ss << std::endl;
            }
            unsigned char hex = buff[i];
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) hex
                    << std::setfill(' ') << std::dec;
        }
        return ss.str();
    }

    static std::string Bin2Hex(const char* buff, int len)
    {
        std::ostringstream ss;

        for (uint32_t i = 0; i < len; i++)
        {
            unsigned char hex = buff[i];
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) hex
                    << std::setfill(' ') << std::dec;
        }
        return ss.str();
    }

    static std::string Bin2Hex(const std::string& bin)
    {
        std::ostringstream ss;

        for (uint32_t i = 0; i < bin.length(); i++)
        {
            unsigned char hex = bin[i];
            ss << std::hex << std::setw(2) << std::setfill('0') << (int) hex
                    << std::setfill(' ') << std::dec;
        }
        return ss.str();
    }

    static int bin_value(char ch)
    {
        if ('0' <= ch && ch <= '9')
        {
            return ch - '0';
        }
        else if ('a' <= ch && ch <= 'z')
        {
            return ch - 'a' + 0x0A;
        }
        else if ('A' <= ch && ch <= 'Z')
        {
            return ch - 'A' + 0x0A;
        }
        else
        {
            return 0;
        }
    }

    static std::string Hex2Bin(const std::string& hex)
    {
        if ((hex.length() % 2) != 0 || hex.empty())
        {
            return "";
        }

        int bin_len = hex.length() / 2;
        char *buffer = new char[bin_len];
        char *p = buffer;

        for (uint32_t i = 0; i < hex.length() - 1; i += 2)
        {
            *p = (char) (((bin_value(hex[i]) << 4) | bin_value(hex[i + 1]))
                    & 0xFF);
            ++p;
        }

        std::string ret(buffer, bin_len);
        delete[] buffer;
        return ret;
    }

    static uint32_t HashString(const char *key, int32_t length)
    {
        register uint32_t nr = 1, nr2 = 4;
        while (length--)
        {
            nr ^= (((nr & 63) + nr2) * ((uint32_t)(uint8_t) * key++))
                    + (nr << 8);
            nr2 += 3;
        }
        if (nr == 0)
        {
            nr = 1;
        }
        return ((uint32_t) nr);
    }

    static void DumpTrace(std::string &errinfo);

    static void Demangle(char * msg, std::string &out);

    //获取本机IP
    static std::string GetLocalIP(const std::string& if_name = "");
};



#endif
