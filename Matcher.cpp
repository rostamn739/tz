#include "Matcher.hpp"

Matcher::return_type Matcher::match_one(const std::string &what, size_t linenum)
const {
    return_type result;
    std::smatch mask_match;
    if (regex_search(what, mask_match, given_mask)) {
        for (size_t i = 0; i < mask_match.size(); ++i) {
            result.push_back({
                                     linenum,
                                     mask_match.position(i) + 1,
                                     mask_match[i]
                             });
        }
    }
    return result;
}
