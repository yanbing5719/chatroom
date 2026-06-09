#include <iostream>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstring>


#include "server.hpp"

    FTPServer:: FTPServer()
    {
        listenfd = -1;
    }
    FTPServer::~FTPServer()
    {
        if (listenfd != -1)
        {
            close(listenfd);
        }
    }
    bool FTPServer::BuildConnect()
    {
        // 创建监听套接字
        listenfd = socket(AF_INET, SOCK_STREAM, 0);

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
        std::cout << "启动服务器，端口号8888" << std::endl;

        // 连接客户端
        while (1)
        {
            int clientfd = accept(listenfd, nullptr, nullptr);
            if (clientfd < 0)
            {
                perror("accept");
                continue;
            }
            std::cout << "客户端连接成功" << std::endl;
        }
        close(listenfd);
        return true;
    }

    // listenfd在该函数内不可修改
    int FTPServer::getlistenfd() const
    {
        return listenfd;
    }