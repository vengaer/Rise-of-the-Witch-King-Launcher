#include "input.h"
#include <regex>
#include <string>
#include <string_view>

bool is_blank_line(std::string_view view);
bool is_header(std::string_view view);
bool is_subheader(std::string_view view);
bool is_key_value_pair(std::string_view view);

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

enum line_contents determine_line_contents(char const* line) {
    std::string const contents{line};
    std::string_view const view{contents};

    if(is_blank_line(view))
        return content_blank;
    if(is_header(view))
        return content_header;
    if(is_subheader(view))
        return content_subheader;
    if(is_key_value_pair(view))
        return content_key_value_pair;

    return content_invalid;
}

bool is_blank_line(std::string_view view) {
    std::regex const rgx{"^\\s*$"};
    svmatch match;
    return std::regex_search(std::begin(view), std::end(view), match, rgx);;
}

bool is_header(std::string_view view) {
    std::regex const rgx{"^\\s*\\[[a-zA-Z0-9.]+\\]\\s*$"};
    svmatch match;
    return std::regex_search(std::begin(view), std::end(view), match, rgx);
}

bool is_subheader(std::string_view view) {
    std::regex const rgx{"^\\s*\\[\\[[a-zA-Z0-9.]+\\]\\]\\s*$"};
    svmatch match;
    return std::regex_search(std::begin(view), std::end(view), match, rgx);
}

bool is_key_value_pair(std::string_view view) {
    std::regex const rgx{"^\\s*[A-Za-z0-9\\_]+\\s*=\\s*\".*\"\\s*$"};
    svmatch match;
    return std::regex_search(std::begin(view), std::end(view), match, rgx);
}
