
#include "Log.h"
#include "../log/Log.cc"
int main()
{
    Logger::init("test.txt");
    Logger().info() << "小莲莲啦啦啦";
    return 0;
}

