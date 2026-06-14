#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>
#include <sys/epoll.h>
#include <vector>

#include "server.hpp"
#include "epoll.hpp"
#include "connection.hpp"

FTPServer::FTPServer()
{
    listenfd = -1;
}
FTPServer::~FTPServer()
{
    //释放掉connections里面的对象
    for(auto &p:connections){
        delete p.second;
    }
    connections.clear();
    if (listenfd != -1)
    {
        close(listenfd);
    }
}
bool FTPServer::BuildConnect()
{
    // 创建监听套接字
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    Epoll epoll;

    // 设置监听地址
    sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(PORT);
    addr.sin_addr.s_addr = INADDR_ANY;

    // 允许在 TIME_WAIT 状态立即重启服务器
    // 允许多个 socket 绑定到同一端口（需配合其他条件）端口复用
    int opt = 1;
    setsockopt(listenfd,
               SOL_SOCKET,
               SO_REUSEADDR,
               &opt,
               sizeof(opt));
    // 绑定地址

    if (bind(listenfd, (sockaddr *)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        listenfd = -1;
        return false;
    }

    // 监听
    if (listen(listenfd, SOMAXCONN) == -1)
    {
        perror("listen");
        listenfd = -1;
        return false;
    }
    if (!epoll.setonnblock(listenfd))
    {
        return false;
    }

    if (!epoll.init())
    {
        return false;
    }

    if (!epoll.add(listenfd, EPOLLIN))
    {
        return false;
    }
    std::cout << "启动服务器，端口号8888" << std::endl;

    // 连接客户端
    while (1)
    {

        std::vector<ft> events = epoll.wait();
        if (events.empty())
        {
            continue;
        }
        int size = events.size();
        for (int i = 0; i < size; i++)
        {
            if (events[i].fd == listenfd)
            {
                sockaddr_in clientaddr;
                socklen_t clientlen = sizeof(clientaddr);

                int clientfd = accept(listenfd, (sockaddr *)&clientaddr, &clientlen);
                if (clientfd < 0)
                {
                    perror("accept");
                    continue;
                }

                // 新连接也需要加入epoll
                if(!epoll.setonnblock(clientfd)){
                    close(clientfd);
                    continue;
                }
                if(!epoll.add(clientfd, EPOLLIN)){
                    close(clientfd);
                    continue;
                }

                Connection *conn = new Connection(clientfd, clientaddr);
                connections[clientfd] = conn;
                std::cout << "客户端连接成功" << std::endl;
            }
            else
            {
                auto it = connections.find(events[i].fd);
                if (it == connections.end())
                {
                    continue;
                }
                Connection *conn = it->second;
                if (!conn->receive())   //该客户端已经没有用处
                {
                    std::cout << "客户端断开连接" << std::endl;
                    epoll.del(events[i].fd);
                    delete conn;
                    connections.erase(it);
                }
            }
        }
    }
    
}
