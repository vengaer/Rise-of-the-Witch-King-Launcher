#include "pattern.h"
#include "string_view_regex.h"
#include <stdio.h>
#include <regex>
#include <string>
#include <string_view>

bool is_blank_line(std::string_view view);
bool is_header(std::string_view view);
bool is_subheader(std::string_view view);
bool is_key_value_pair(std::string_view view);

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

void version_introduced_in(char* dst, char const* filename) {
    std::string const contents{filename};
    std::string_view view{contents};

    std::regex const rgx{"(v[0-9](\\.[0-9]){2,3})\\.[a-zA-Z]{1,4}$"};
    svmatch match;

    if(std::regex_search(std::begin(view), std::end(view), match, rgx) && match.size() > 1)
        memcpy(dst, match[1].str().data(), match[1].length());
    else
        dst[0] = '\0';
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
    std::regex const rgx{"^\\s*[A-Za-z0-9\\_.]+\\s*=\\s*\".*\"\\s*$"};
    svmatch match;
    return std::regex_search(std::begin(view), std::end(view), match, rgx);
}
