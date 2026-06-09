#pragma once

#include <mutex>
#include <condition_variable>
#include <vector>
#include <queue>
#include <functional>
#include <thread>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex mtx;
    bool stop;
    std::condition_variable cv;

public:
    ThreadPool(int threadnum);
    ~ThreadPool();
    void enqueue(std::function<void()> task);
};