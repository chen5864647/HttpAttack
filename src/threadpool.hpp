#ifndef THREADPOOL_HPP_
#define THREADPOOL_HPP_

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <functional>
#include <mutex>
#include <condition_variable>
#include <future>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool(size_t);
    
    template <class F, class ... Args>
    auto enqueue(F &&f, Args &&... args) -> std::future<typename std::result_of<F (Args...)>::type>;
    ~ThreadPool();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void ()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    bool stop;

};

#endif  // THREADPOOL_HPP_