#include "threadpool.hpp"

ThreadPool::ThreadPool(int threadnum) : stop(false)
{
    for (int i = 0; i < threadnum; i++)
    {
        threads.emplace_back([this]()
                             {
                while(1){
                    std::function<void()> task;
                    {
                        std::unique_lock<std::mutex> lock(mtx);
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

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(mtx);
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

void ThreadPool::enqueue(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(mtx);
        tasks.push(task);
    }
    cv.notify_one();
}
