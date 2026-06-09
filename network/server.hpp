#pragma once

class FTPServer
{
private:
    int listenfd;
    const int PORT = 8888;

public:
    FTPServer();
    ~FTPServer();
    bool BuildConnect();
    int getlistenfd()const;
};