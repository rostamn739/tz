#ifndef TZ_WORKERPOOL_HPP
#define TZ_WORKERPOOL_HPP

#include <vector>
#include <thread>
#include <functional>
#include <mutex>
#include <queue>
#include <condition_variable>
#include <future>

using std::size_t;

/**
 * a quick worker pool for tasks
 * based on progschj/ThreadPool on github
 *
 */
class WorkerPool {
    using task_type = std::function<void()>;
    std::mutex tasks_mutex;
    std::condition_variable tasks_cv;
    std::queue<task_type> tasks_queue;

    std::vector<std::thread> workers;
    bool stopping = false;
public:
    template<typename TFunc, typename... TArgs>
    auto post(TFunc &&func, TArgs &&... args) -> std::future<
            typename std::result_of_t<TFunc(TArgs...)>
    > {
        using ret_t = typename std::result_of_t<TFunc(TArgs...)>;
        auto task = std::make_shared<
                std::packaged_task<ret_t()>>(
                std::bind(std::forward<TFunc>(func), std::forward<TArgs>(args)...)
        );
        std::future<ret_t> result = task->get_future();
        {
            std::lock_guard<std::mutex> lock(tasks_mutex);
            if (stopping) throw std::runtime_error("Enqueue on stopping pool");
            tasks_queue.emplace(
                    [task = std::move(task)] {
                        (*task)();
                    }
            );
        }
        tasks_cv.notify_one();
        return result;
    }

    explicit WorkerPool(size_t count);

    ~WorkerPool();
};


#endif //TZ_WORKERPOOL_HPP
