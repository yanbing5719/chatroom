#include <iostream>
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
    ThreadPool(int threadnum) : stop(false)
    {
        for (int i = 0; i < threadnum; i++)
        {
            threads.emplace_back([this]()
                                 {
                while(1){
                    std::function<void()> task;
                    {
                        std::unique_lock lock(mtx);
                        cv.wait(lock,[this](){
                            return !tasks.empty()||stop;
                        });
                        if(stop&&tasks.empty()){
                            return ;
                        }
                        task=move(tasks.front());
                        tasks.pop();
                    }
                    task();
                
                } });
        }
    }

    ~ThreadPool()
    {
        {
            std::unique_lock lock(mtx);
            stop = true;
        }
        for (auto &t : threads)
        {
            if (t.joinable())
            {
                // 等待所有线程结束
                t.join();
            }
        }
    }

    void enqueue(std::function<void()> task)
    {
        {
            std::unique_lock lock(mtx);
            tasks.push(task);
        }
        cv.notify_one();
    }
};