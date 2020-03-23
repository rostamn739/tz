#include <iostream>
#include <fstream>

#include "WorkerPool.hpp"
#include "Matcher.hpp"
#include "Batcher.hpp"


namespace utility {

unsigned int
default_concurrency() {
    unsigned conc = std::thread::hardware_concurrency();
    if (!conc) return 4;
    return conc;
}
/**
 * basically replace question marks with dots
 */
std::regex parse_mask(std::string mask) {
    std::string result;
    result = std::regex_replace(mask, std::regex("\\."), "\\.");
    result = std::regex_replace(result, std::regex("\\?"), ".");
    return std::regex(result);
}
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: tz filename mask\n";
        return 1;
    }
    std::regex mask = utility::parse_mask(argv[2]);
    std::ifstream infile(argv[1]);
    if (!infile){
        std::cerr << "Could not open text file\n";
        return 1;
    }
    WorkerPool wpool{utility::default_concurrency()};
    using utility::make_batcher;
    auto batcher = make_batcher(
            wpool,
            [&infile] {
                std::string line;
                if (std::getline(infile, line)) {
                    if (line.empty()) return std::string(" ");
                    return line;
                }
                return std::string{};
            }
            );
    Matcher matcher{mask};
    auto run = batcher.run_matches(matcher);
    auto match_results = std::vector<Matcher::MatchResult>{};
    for (auto &&fut : run) {
        auto results = fut.get();
        for (const auto &mr : results) {
            match_results.push_back(mr);
        }
    }
    // print first line of input, i.e. match vector size:
    std::cout << match_results.size() << "\n";
    for (const auto &mr : match_results) {
        // print stats for each match
        std::cout << mr.linenum << " ";
        std::cout << mr.matchpos << " ";
        std::cout << mr.entry << "\n";
    }
    return 0;
}
