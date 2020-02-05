//
// Created by kiosk on 2020/1/31.
//
#include "src/client/Client.h"

int main(){
    Provider::Get();
    Client c;
    c.Run();
    return 0;
}
