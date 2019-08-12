#ifndef STRING_VIEW_REGEX_H
#define STRING_VIEW_REGEX_H

#include <regex>
#include <string_view>

using svmatch = std::match_results<std::string_view::const_iterator>;
using svsub_match = std::sub_match<std::string_view::const_iterator>;

inline std::string_view get_sv(svsub_match const& m) {
    return std::string_view(m.first, m.length());
}

inline bool regex_match(std::string_view sv, svmatch& m, std::regex const& e, std::regex_constants::match_flag_type flags = std::regex_constants::match_default) {
    return std::regex_match(std::begin(sv), std::end(sv), m, e, flags);
}

inline bool regex_match(std::string_view sv, std::regex const& e, std::regex_constants::match_flag_type flags = std::regex_constants::match_default) {
    return std::regex_match(std::begin(sv), std::end(sv), e, flags);
}

#endif
