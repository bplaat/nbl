#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "utils.h"

char *string_copy(char *string) {
    char *new_string = malloc(strlen(string) + 1);
    strcpy(new_string, string);
    return new_string;
}

char *string_concat(char *left_string, char *right_string) {
    char *new_string = malloc(strlen(left_string) + strlen(right_string) + 1);
    strcpy(new_string, left_string);
    strcat(new_string, right_string);
    return new_string;
}

char *string_append(char *left_string, char *right_string) {
    char *new_string = realloc(left_string, strlen(left_string) + strlen(right_string) + 1);
    strcat(new_string, right_string);
    return new_string;
}

char *string_format(char *format, ...) {
    va_list args;
    va_start(args, format);

    va_list args2;
    va_copy(args2, args);
    char *new_string = malloc(vsnprintf(NULL, 0, format, args2) + 1);
    va_end(args2);

    vsprintf(new_string, format, args);
    va_end(args);
    return new_string;
}

char *file_read(char *path) {
    FILE *file = fopen(path, "r");
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *file_buffer = malloc(file_size + 1);
    fread(file_buffer, 1, file_size, file);
    file_buffer[file_size] = '\0';
    fclose(file);
    return file_buffer;
}
