#include "threadPool.hpp"

threadPool::threadPool() 
{
    auto cores = std::thread::hardware_concurrency();

    auto threads = std::max(1u, cores-1);

    for (size_t i = 0; i < threads; ++i) {
        m_threads.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(this->m_queueMutex);
                    this->m_conditionVariable.wait(lock, [this] {
                        return this->m_stop || !this->m_tasks.empty();
                    });
                    if (this->m_stop && this->m_tasks.empty()) return;
                    task = std::move(this->m_tasks.front());
                    this->m_tasks.pop();
                }
                task();
            }
        });
    }
}