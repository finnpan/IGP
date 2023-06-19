// copied from: https://github.com/progschj/ThreadPool

#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <vector>
#include <queue>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <functional>
#include <stdexcept>

class ThreadPool {
public:
    ThreadPool (size_t);

    template<class F, class... Args>
    auto enqueue (F&& f, Args&&... args)
        -> std::future<typename std::result_of<F(Args...)>::type>;

    ~ThreadPool ();

private:
    // need to keep track of threads so we can join them
    std::vector<std::thread> _workers;

    // the task queue
    std::queue<std::function<void()>> _tasks;
    
    // synchronization
    std::mutex _queue_mutex;
    std::condition_variable _condition;
    bool _stop;
};

// the constructor just launches some amount of workers
inline ThreadPool::ThreadPool (size_t threads)
    : _stop(false)
{
    for (size_t i = 0; i < threads; ++i) {
        this->_workers.emplace_back([this]
        {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock<std::mutex> lock(this->_queue_mutex);
                    this->_condition.wait(lock,
                        [this]{ return this->_stop || !this->_tasks.empty(); });
                    if (this->_stop && this->_tasks.empty()) {
                        return;
                    }
                    task = std::move(this->_tasks.front());
                    this->_tasks.pop();
                }

                task();
            }
        });
    }
}

// add new work item to the pool
template<class F, class... Args>
auto ThreadPool::enqueue (F&& f, Args&&... args)
    -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;

    auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...)
        );

    std::future<return_type> res = task->get_future();
    {
        std::unique_lock<std::mutex> lock(this->_queue_mutex);

        // don't allow enqueueing after stopping the pool
        if (this->_stop) {
            throw std::runtime_error("enqueue on stopped ThreadPool");
        }

        this->_tasks.emplace([task](){ (*task)(); });
    }
    this->_condition.notify_one();
    return res;
}

// the destructor joins all threads
inline ThreadPool::~ThreadPool ()
{
    {
        std::unique_lock<std::mutex> lock(this->_queue_mutex);
        this->_stop = true;
    }

    this->_condition.notify_all();
    for (std::thread& worker : this->_workers) {
        worker.join();
    }
}

#endif
