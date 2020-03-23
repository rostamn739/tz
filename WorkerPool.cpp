#include "WorkerPool.hpp"

WorkerPool::WorkerPool(size_t count) {
    for (size_t i = 0; i < count; ++i) {
        workers.emplace_back([this] {
            for (;;) {
                task_type task;
                {
                    std::unique_lock<std::mutex> lock(tasks_mutex);
                    tasks_cv.wait(
                            lock,
                            [this] { return stopping || !tasks_queue.empty(); });
                    if (stopping && tasks_queue.empty()) return;
                    task = std::move(tasks_queue.front());
                    tasks_queue.pop();
                }
                task();
            }
        });
    }
}

WorkerPool::~WorkerPool() {
    {
        std::lock_guard<std::mutex> lock(tasks_mutex);
        stopping = true;
    }
    tasks_cv.notify_all();
    for (auto &&w : workers) {
        w.join();
    }
}
