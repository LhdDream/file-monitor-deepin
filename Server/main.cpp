#include <csignal>

#include "thread_pool/threadpool.h"
int main() {
    signal(SIGPIPE, SIG_IGN);
    Proxy l;
    return l.wait();
}