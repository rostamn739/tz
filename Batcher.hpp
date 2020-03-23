#ifndef TZ_BATCHER_HPP
#define TZ_BATCHER_HPP

#include <utility>

#include "Matcher.hpp"
#include "WorkerPool.hpp"

using std::size_t;

template<typename TLineFunc>
class Batcher {
    WorkerPool &pool;
    TLineFunc line_func;
public:
    using result_type = Matcher::return_type;
    using return_type = std::vector<std::future<result_type>>;

    Batcher(WorkerPool &worker_pool, TLineFunc &&line_f)
            : pool(worker_pool),
              line_func{std::forward<TLineFunc>(line_f)} {
        //
    }

    return_type run_matches(const Matcher &matcher) {
        return_type final_results;
        size_t target_batch_size = 1;
        size_t current_batched = 0;
        size_t current_linenum = 0;
        size_t last_linenum = current_linenum + 1;
        std::vector<std::string> work_args;
        for (std::string line = line_func();
             !line.empty();
             line = line_func()) {
            work_args.push_back(std::move(line));
            current_batched++;
            current_linenum++;
            if (current_batched == target_batch_size) {
                auto tmp__ = pool.post(
                        [work_args = std::move(work_args),
                                &matcher,
                                last_linenum] {
                            result_type results;
                            auto linenum = last_linenum;
                            for (const std::string &line : work_args) {
                                auto line_results =
                                        matcher.match_one(line, linenum);
                                for (const auto &item : line_results) {
                                    results.push_back(item);
                                }
                                linenum++;
                            }
                            return results;
                        });
                final_results.push_back(std::move(tmp__));
                work_args = std::vector<std::string>{};
                current_batched = 0;
                target_batch_size *= 2;
                last_linenum = current_linenum + 1;
            }
        }
        if (!work_args.empty()) {
            auto tmp__ = pool.post(
                    [work_args = std::move(work_args),
                            &matcher,
                            last_linenum] {
                        result_type results;
                        auto linenum = last_linenum;
                        for (const std::string &line : work_args) {
                            auto line_results =
                                    matcher.match_one(line, linenum);
                            for (const auto &item : line_results) {
                                results.push_back(item);
                            }
                            linenum++;
                        }
                        return results;
                    });
            final_results.push_back(std::move(tmp__));
//            work_args = std::vector<std::string>{};
        }
        return final_results;
    }
};

namespace utility {
template <typename TLineFunc>
Batcher<TLineFunc> make_batcher(WorkerPool &pool, TLineFunc &&func) {
    return { pool, std::forward<TLineFunc>(func) };
}
}


#endif //TZ_BATCHER_HPP
