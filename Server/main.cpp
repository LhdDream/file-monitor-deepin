#include <csignal>
#include "Server/Server.h"
int main(){
    signal(SIGPIPE,SIG_IGN);
    Server l;
    l.Run();
}