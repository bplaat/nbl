#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *strdup(const char *str) { return strndup(str, strlen(str)); }

char *strndup(const char *str, size_t size) {
    char *copy = malloc(size + 1);
    if (!copy) return NULL;
    memcpy(copy, str, size);
    copy[size] = '\0';
    return copy;
}

char *nbl_file_read(char *path) {
    FILE *file = fopen(path, "rb");
    if (!file) return NULL;
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *buffer = malloc(size + 1);
    size = fread(buffer, 1, size, file);
    buffer[size] = '\0';
    fclose(file);
    return buffer;
}
