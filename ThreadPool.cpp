#include "ThreadPool.hpp"


ThreadPool::ThreadPool(int threadCount) : threadCount(threadCount), stop(false) {
    for (int i = 0; i < threadCount ; i++){
        workers.emplace_back([this](){
            while(true){
                std::unique_lock<std::mutex> lock(this->mutex);
                //Wait again if the not finishes
                this->cv.wait(lock,[this]() {
                    return this->stop || !this->tasks.empty();
                });
                // woken up but no tasks or program finished?
                if (this->stop && this->tasks.empty()) {
                    return; 
                }

                // we use move to avoid creating intermediate object by directly transfering ownership
                // ****
                std::function<void()> task = std::move(tasks.front());

                tasks.pop();

                lock.unlock();

                task();

            }
        }); 
    }
}

ThreadPool::~ThreadPool(){
    //if we dont lock before write the value(though it will only be modified by main thread)
    // the value store on the cache will not be pushed to the ram, so other threads wont notice it changed
    std::unique_lock<std::mutex> lock(this->mutex);
    this->stop = true;
    lock.unlock();

    cv.notify_all();

    for(auto & worker : workers){
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::queuTask(std::function<void()> task){
    std::unique_lock<std::mutex> lock(this->mutex);
    // same strategy than ****
    tasks.push(std::move(task));
    lock.unlock();
    this->cv.notify_one();
    
}
