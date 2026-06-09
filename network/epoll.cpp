#include "epoll.hpp"

#include <iostream>
#include <sys/epoll.h>
#include <unistd.h>

Epoll::Epoll()
{
    epfd = -1;
}

Epoll::~Epoll()
{
    if (epfd != -1)
    {
        close(epfd);
    }
}

bool Epoll::init(int listenfd)
{
    if (listenfd < 0)
    {
        perror("listenfd");
        return false;
    }
    // 创建epoll实例（不使用任何标志）
    epfd = epoll_create1(0);
    if (epfd < 0)
    {
        perror("epoll_creat");
        return false;
    }

    // 定义事件
    struct epoll_event ev;
    ev.events = EPOLLIN;
    ev.data.fd = listenfd;

    // 将listenfd添加到epoll实例里面
    if (epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &ev) == -1)
    {
        perror("epoll_ctl");
        return false;
    }

    return true;
}

bool Epoll::epoll(int listenfd)
{
    struct epoll_event evs[1024];
    int size = sizeof(evs) / sizeof(evs[0]);
    while (1)
    {

        // num为符合条件的fd的数量,-1无限等待
        int num = epoll_wait(epfd, evs, size, -1);
        for (int i = 0; i < num; i++)
        {
            int fd = evs[i].data.fd;

            // 用来监听的文件描述符
            if (fd == listenfd){
                
            }
        }
    }
}