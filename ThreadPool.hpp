#pragma once

#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

class ThreadPool {
    private:
        std::vector<std::thread> workers;
        std::queue<std::function<void()>> tasks;
        std::mutex mutex;
        std::condition_variable cv;
        bool stop;
        int threadCount;


    public:
        ThreadPool(int threadCount);
        ~ThreadPool();
        void queuTask(std::function<void()> task);


    
};