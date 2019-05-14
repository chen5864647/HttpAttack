#include <iostream>
#include "threadpool.hpp"

/*
inline
ThreadPool::ThreadPool(size_t threads) : stop(false) {

    for (size_t i = 0; i < threads; ++i) 
        workers.emplace_back(
            [this] {
                for (;;) {
                    std::function<void ()> task;
                    {
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this] {
                                return this->stop || !this->tasks.empty();
                            });
                        
                        if (this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task();
                }
            }
        );

}

template <class F, class ... Args>
auto ThreadPool::enqueue(F &&f, Args &&... args) 
    -> std::future<typename std::result_of<F (Args...)>::type> {

    using return_type = typename std::result_of<F (Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...);
    );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        if (stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");
        
        tasks.emplace([task]() {
            (*task)();
        });
    }
    condition.notify_one();
    return res;
}

inline
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();

    for (std::thread &worker : workers)
        worker.join();
}

*/

// the constructor just launches some amount of workers
inline 
ThreadPool::ThreadPool(size_t threads): stop(false) {
    for(size_t i = 0;i<threads;++i)
        workers.emplace_back(     //以下为构造一个任务，即构造一个线程
            [this] {
                for(;;) {
                    std::function<void()> task;   //线程中的函数对象
                    {//大括号作用：临时变量的生存期，即控制lock的时间
                        std::unique_lock<std::mutex> lock(this->queue_mutex);
                        this->condition.wait(lock,
                            [this]{ return this->stop || !this->tasks.empty(); }); //当stop==false&&tasks.empty(),该线程被阻塞 !this->stop&&this->tasks.empty()
                        if(this->stop && this->tasks.empty())
                            return;
                        task = std::move(this->tasks.front());
                        this->tasks.pop();
                    }

                    task(); //调用函数，运行函数
                }
            }
        );
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue(F&& f, Args&&... args)  //&& 引用限定符，参数的右值引用，  此处表示参数传入一个函数
-> std::future<typename std::result_of<F(Args...)>::type> {
    using return_type = typename std::result_of<F(Args...)>::type;
     //packaged_task是对任务的一个抽象，我们可以给其传递一个函数来完成其构造。之后将任务投递给任何线程去完成，通过
//packaged_task.get_future()方法获取的future来获取任务完成后的产出值
    auto task = std::make_shared<std::packaged_task<return_type()> >(  //指向F函数的智能指针
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)  //传递函数进行构造
        );
    //future为期望，get_future获取任务完成后的产出值
    std::future<return_type> res = task->get_future();   //获取future对象，如果task的状态不为ready，会阻塞当前调用者
    {
        std::unique_lock<std::mutex> lock(queue_mutex);  //保持互斥性，避免多个线程同时运行一个任务

        // don't allow enqueueing after stopping the pool
        if(stop)
            throw std::runtime_error("enqueue on stopped ThreadPool");

        tasks.emplace([task](){ (*task)(); });  //将task投递给线程去完成，vector尾部压入
    }
    condition.notify_one();  //选择一个wait状态的线程进行唤醒，并使他获得对象上的锁来完成任务(即其他线程无法访问对象)
    return res;
}//notify_one不能保证获得锁的线程真正需要锁，并且因此可能产生死锁

// the destructor joins all threads
inline 
ThreadPool::~ThreadPool() {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        stop = true;
    }
    condition.notify_all();  //通知所有wait状态的线程竞争对象的控制权，唤醒所有线程执行
    for(std::thread &worker: workers)
        worker.join(); //因为线程都开始竞争了，所以一定会执行完，join可等待线程执行完
}

void fun() {
    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    std::cout << "worker thread ID: " << std::this_thread::get_id() << std::endl;
}
