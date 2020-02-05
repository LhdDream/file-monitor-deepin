//
// Created by kiosk on 2020/2/3.
//

#include "File.h"

void File::ModifyFile(const std::string &m_path) {
    int fd = open(m_path.c_str(), O_TRUNC | O_WRONLY);
    int ret = write(fd, m_wirte, sizeof(m_wirte));
    assert(ret != -1);
}

void File::ReadySend(const std::string &path, const std::unique_ptr<thread_pool> &pool) {
    struct stat64 st{};
    if (stat64(path.data(), &st) < 0) {
        return;
    }
    int filefd = open(path.c_str(), O_RDWR,0666);
    //mmap
    auto memPtr = reinterpret_cast<char *> (mmap(nullptr, st.st_size, PROT_READ, MAP_SHARED, filefd, 0));
    close(filefd);
    auto n = (st.st_size / 8192 + 1 );
    struct data temp[n];
    auto ptr = 0;
    for (size_t i= 0 ; i < n ; i++) {
        bzero(&temp[i], sizeof(struct data));
        auto len =  (st.st_size > (8192 * (i+1))) ? sizeof(temp[i].content) - 1 : st.st_size - 8192 * i;
        strcpy(temp[i].path, path.c_str());
        memmove(temp[i].content, memPtr + ptr , len);
        strcpy(temp[i].mac, Provider::Get().GetMac().c_str());
        temp[i].sign = false;
        temp[i].n = ptr;
        ptr += len;

    }
    for(size_t i = 0 ;i < n; i++) {
        pool->AddTask([&]() {
            Socket c{};
            c.Createfd(Provider::Get().GetIp().c_str(),Provider::Get().GetPort());
            c.Connect();
            if (c.StateConnect()) {
                int re = c.Send(reinterpret_cast<const void *>( &temp[i]), sizeof(struct data), 0);
                printf("Send  %s\n", strerror(errno));
            }
        });
    }
    munmap(memPtr, st.st_size);
}

void File::RecvFile(Socket &&client) {
    while (true) {
        std::unique_ptr<struct data> Data = std::make_unique<struct data>();
        auto  count = client.Recv(Data.get() , sizeof(struct data) , 0);
        if (count == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                break;
            }
            return;
        } else if (count == 0) {
            client.SetState(Socket::K_Break_Link);
        } else {
            //进行文件的写回操作
            for(auto & j : Data->path){
                if(j == '@'){
                    j = '/';
                }
            }
            int fd = open(Data->path, O_CREAT | O_RDWR | O_TRUNC );
            lseek(fd, Data->n , SEEK_SET);
            int Count = write(fd, Data->content, strlen(Data->content));
            close(fd);
        }
    }
}

void File::SendClose(int client, const std::string &name) {
    struct data temp{};
    temp.n = 0;
    temp.sign = true;
    strcpy(temp.mac, Provider::Get().GetMac().c_str());
    strcpy(temp.path, name.c_str());
    int i = ::send(client,&temp, sizeof(struct data), 0);
    if(i < 0){
        printf("%s\n",strerror(errno));
    }
}