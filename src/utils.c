#include <stdlib.h>
#include <string.h>

#include "utils.h"

char *string_copy(char *string) {
    char *new_string = malloc(strlen(string) + 1);
    strcpy(new_string, string);
    return new_string;
}

char *string_concat(char *a, char *b) {
    char *new_string = malloc(strlen(a) + strlen(b) + 1);
    strcpy(new_string, a);
    strcat(new_string, b);
    return new_string;
}
