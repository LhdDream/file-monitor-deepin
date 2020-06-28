#include "log_utils.h"
#include <cxxabi.h>
#include <execinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <unistd.h>
using namespace std;

#define DUMP_STACK_DEPTH_MAX 50


void LogUtils::DumpTrace(string &errinfo)
{
    void *stack_trace[DUMP_STACK_DEPTH_MAX] =
    { 0 };
    char **stack_strings = NULL;
    int stack_depth = 0;
    int i = 0;
    char index[5];

    /* 获取栈中各层调用函数地址 */
    stack_depth = backtrace(stack_trace, DUMP_STACK_DEPTH_MAX);

    /* 查找符号表将函数调用地址转换为函数名称 */
    stack_strings = (char **) backtrace_symbols(stack_trace, stack_depth);
    if (NULL == stack_strings)
    {
        errinfo += " Memory is not enough while dump Stack Trace! \n";
        return;
    }

    /* 保存调用栈 */
    errinfo += "Backtrace:\n\t";
    for (i = 1; i < stack_depth; ++i)
    {
        snprintf(index, sizeof(index), "#%02d ", i);
        errinfo += index;
        Demangle(stack_strings[i], errinfo);
        errinfo += "\n\t";
    }

    /* 获取函数名称时申请的内存需要自行释放 */
    free(stack_strings);
    stack_strings = NULL;

    return;
}

void LogUtils::Demangle(char * msg, string &out)
{
    char *mangled_name = 0, *offset_begin = 0, *offset_end = 0;

    // find parantheses and +address offset surrounding mangled name
    for (char *p = msg; *p; ++p)
    {
        if (*p == '(')
        {
            mangled_name = p;
        }
        else if (*p == '+')
        {
            offset_begin = p;
        }
        else if (*p == ')')
        {
            offset_end = p;
            break;
        }
    }
    // if the line could be processed, attempt to demangle the symbol
    if (mangled_name && offset_begin && offset_end
            && mangled_name < offset_begin)
    {
        *mangled_name++ = '\0';
        *offset_begin++ = '\0';
        *offset_end++ = '\0';

        int status;
        char * real_name = abi::__cxa_demangle(mangled_name, 0, 0, &status);

        // if demangling is successful, output the demangled function name
        if (status == 0)
        {
            out = out + real_name + "+" + offset_begin + offset_end;
        }
        // otherwise, output the mangled function name
        else
        {
            out = out + mangled_name + "+" + offset_begin + offset_end;
        }
        free(real_name);
    }
    else
        // otherwise, save the whole line
        out += msg;
}

string LogUtils::GetLocalIP(const string& if_name)
{
    if (if_name.empty())
    {
        string ip = GetLocalIP("eth1");
        if(ip.empty())
        {
            return GetLocalIP("eth0");
        }
        return ip;
    }
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct ifreq ifr;
    memset(ifr.ifr_name, 0, IFNAMSIZ);
    strncpy(ifr.ifr_name, if_name.c_str(), IFNAMSIZ - 1);
    char buf[32] =
    { 0 };
    if (!ioctl(fd, SIOCGIFADDR, &ifr))
    {
        inet_ntop(AF_INET, &(((struct sockaddr_in*) (&ifr.ifr_addr))->sin_addr),
                buf, 32);
    }
    close(fd);
    return buf;
}


