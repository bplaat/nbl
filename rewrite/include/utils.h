#pragma once

#include <stddef.h>

char *strdup(const char *str);

char *strndup(const char *str, size_t size);

char *nbl_file_read(char *path);
