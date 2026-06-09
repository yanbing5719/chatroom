#pragma once

class Epoll
{
private:
    int epfd;

public:
    Epoll();
    bool init(int listenfd);
    ~Epoll();
    bool epoll(int listenfd);
};