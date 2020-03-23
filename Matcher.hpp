#ifndef TZ_MATCHER_HPP
#define TZ_MATCHER_HPP

#include <string>
#include <vector>
#include <regex>

using std::size_t;

class Matcher {
    std::regex &given_mask;
public:
    struct MatchResult {
        size_t linenum;
        long matchpos;
        std::string entry;
    };
    using return_type = std::vector<MatchResult>;

    explicit Matcher(std::regex &mask)
    : given_mask(mask) {
        //
    }

    return_type match_one(const std::string &what, size_t linenum) const;
};

#endif //TZ_MATCHER_HPP
