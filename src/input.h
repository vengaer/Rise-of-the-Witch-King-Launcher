#ifndef INPUT_H
#define INPUT_H

#ifdef __cplusplus
extern "C" {
#endif

enum line_contents {
    content_invalid,
    content_blank,
    content_header,
    content_subheader,
    content_key_value_pair
};

enum line_contents determine_line_contents(char const* line);

#ifdef __cplusplus
}
#endif

#endif
