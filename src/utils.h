#ifndef UTILS_H
#define UTILS_H

char *string_copy(char *string);

char *string_concat(char *left_string, char *right_string);

char *string_append(char *left_string, char *right_string);

char *string_format(char *format, ...);

char *file_read(char *path);

#endif
