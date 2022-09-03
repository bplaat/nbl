// New Bastiaan Language Interpreter
#include "nbl.h"

// Utils
size_t align(size_t size, size_t alignment) { return (size + alignment - 1) / alignment * alignment; }

void error(char *text, size_t line, size_t position, char *fmt, ...) {
    fprintf(stderr, "text:%lu:%lu ERROR: ", line + 1, position + 1);
    va_list args;
    va_start(args, fmt);
    vfprintf(stderr, fmt, args);
    va_end(args);

    // Seek to the right line in text
    char *c = text;
    for (size_t i = 0; i < line; i++) {
        while (*c != '\n' && *c != '\r') c++;
        if (*c == '\r') c++;
        c++;
    }
    char *lineStart = c;
    while (*c != '\n' && *c != '\r') c++;
    size_t lineLength = c - lineStart;

    fprintf(stderr, "\n%4lu | ", line + 1);
    fwrite(lineStart, 1, lineLength, stderr);
    fprintf(stderr, "\n     | ");
    for (size_t i = 0; i < position; i++) fprintf(stderr, " ");
    fprintf(stderr, "^\n");
    exit(EXIT_FAILURE);
}

// List
List *list_new(void) { return list_new_with_capacity(8); }

List *list_new_with_capacity(size_t capacity) {
    List *list = malloc(sizeof(List));
    list->refs = 1;
    list->items = malloc(sizeof(void *) * capacity);
    list->capacity = capacity;
    list->size = 0;
    return list;
}

List *list_ref(List *list) {
    list->refs++;
    return list;
}

void *list_get(List *list, size_t index) {
    if (index < list->size) {
        return list->items[index];
    }
    return NULL;
}

void list_set(List *list, size_t index, void *item) {
    if (index >= list->capacity) {
        while (index >= list->capacity) list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    if (index > list->size) {
        for (size_t i = list->size; i < index; i++) {
            list->items[i] = NULL;
        }
        list->size = index;
    }
    list->items[index] = item;
}

void list_add(List *list, void *item) {
    if (list->size == list->capacity) {
        list->capacity *= 2;
        list->items = realloc(list->items, sizeof(void *) * list->capacity);
    }
    list->items[list->size++] = item;
}

void list_free(List *list, ListFreeFunc *freeFunc) {
    list->refs--;
    if (list->refs > 0) return;

    if (freeFunc != NULL) {
        for (size_t i = 0; i < list->size; i++) {
            freeFunc(list->items[i]);
        }
    }
    free(list->items);
    free(list);
}

// Map
Map *map_new(void) { return map_new_with_capacity(8); }

Map *map_new_with_capacity(size_t capacity) {
    Map *map = malloc(sizeof(Map));
    map->refs = 1;
    map->keys = malloc(sizeof(char *) * capacity);
    map->values = malloc(sizeof(void *) * capacity);
    map->capacity = capacity;
    map->size = 0;
    return map;
}

Map *map_ref(Map *map) {
    map->refs++;
    return map;
}

void *map_get(Map *map, char *key) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            return map->values[i];
        }
    }
    return NULL;
}

void map_set(Map *map, char *key, void *item) {
    for (size_t i = 0; i < map->size; i++) {
        if (!strcmp(map->keys[i], key)) {
            map->values[i] = item;
            return;
        }
    }

    if (map->size == map->capacity) {
        map->capacity *= 2;
        map->keys = realloc(map->keys, sizeof(char *) * map->capacity);
        map->values = realloc(map->values, sizeof(void *) * map->capacity);
    }
    map->keys[map->size] = strdup(key);
    map->values[map->size] = item;
    map->size++;
}

void map_free(Map *map, MapFreeFunc *freeFunc) {
    map->refs--;
    if (map->refs > 0) return;

    for (size_t i = 0; i < map->size; i++) {
        free(map->keys[i]);
        if (freeFunc != NULL) {
            freeFunc(map->values[i]);
        }
    }
    free(map->keys);
    free(map->values);
    free(map);
}

// Lexer
Token *token_new(TokenType type, size_t line, size_t position) {
    Token *token = malloc(sizeof(Token));
    token->refs = 1;
    token->type = type;
    token->line = line;
    token->position = position;
    return token;
}

Token *token_new_int(size_t line, size_t position, int64_t integer) {
    Token *token = token_new(TOKEN_INT, line, position);
    token->integer = integer;
    return token;
}

Token *token_new_float(size_t line, size_t position, double floating) {
    Token *token = token_new(TOKEN_FLOAT, line, position);
    token->floating = floating;
    return token;
}

Token *token_new_string(TokenType type, size_t line, size_t position, char *string) {
    Token *token = token_new(type, line, position);
    token->string = string;
    return token;
}

Token *token_ref(Token *token) {
    token->refs++;
    return token;
}

bool token_type_is_type(TokenType type) {
    return type == TOKEN_TYPE_ANY || type == TOKEN_NULL || type == TOKEN_TYPE_BOOL || type == TOKEN_TYPE_INT || type == TOKEN_TYPE_FLOAT ||
           type == TOKEN_TYPE_STRING || type == TOKEN_TYPE_ARRAY || type == TOKEN_TYPE_OBJECT || type == TOKEN_TYPE_FUNCTION;
}

char *token_type_to_string(TokenType type) {
    if (type == TOKEN_KEYWORD) return "keyword";
    if (type == TOKEN_INT) return "int";
    if (type == TOKEN_FLOAT) return "float";
    if (type == TOKEN_STRING) return "string";

    if (type == TOKEN_EOF) return "EOF";
    if (type == TOKEN_LPAREN) return "(";
    if (type == TOKEN_RPAREN) return ")";
    if (type == TOKEN_LCURLY) return "{";
    if (type == TOKEN_RCURLY) return "}";
    if (type == TOKEN_LBRACKET) return "[";
    if (type == TOKEN_RBRACKET) return "]";
    if (type == TOKEN_SEMICOLON) return ";";
    if (type == TOKEN_COLON) return ":";
    if (type == TOKEN_COMMA) return ",";
    if (type == TOKEN_POINT) return ".";
    if (type == TOKEN_FAT_ARROW) return "=>";

    if (type == TOKEN_ASSIGN) return "=";
    if (type == TOKEN_ADD) return "+";
    if (type == TOKEN_ASSIGN_ADD) return "+=";
    if (type == TOKEN_SUB) return "-";
    if (type == TOKEN_ASSIGN_SUB) return "-=";
    if (type == TOKEN_MUL) return "*";
    if (type == TOKEN_ASSIGN_MUL) return "*=";
    if (type == TOKEN_EXP) return "**";
    if (type == TOKEN_ASSIGN_EXP) return "**=";
    if (type == TOKEN_DIV) return "/";
    if (type == TOKEN_ASSIGN_DIV) return "/=";
    if (type == TOKEN_MOD) return "%";
    if (type == TOKEN_ASSIGN_MOD) return "%=";
    if (type == TOKEN_AND) return "&";
    if (type == TOKEN_ASSIGN_AND) return "&=";
    if (type == TOKEN_XOR) return "^";
    if (type == TOKEN_ASSIGN_XOR) return "^=";
    if (type == TOKEN_OR) return "|";
    if (type == TOKEN_ASSIGN_OR) return "|=";
    if (type == TOKEN_NOT) return "~";
    if (type == TOKEN_SHL) return "<<";
    if (type == TOKEN_ASSIGN_SHL) return "<<=";
    if (type == TOKEN_SHR) return ">>";
    if (type == TOKEN_ASSIGN_SHR) return ">>=";
    if (type == TOKEN_EQ) return "==";
    if (type == TOKEN_NEQ) return "!=";
    if (type == TOKEN_LT) return "<";
    if (type == TOKEN_LTEQ) return "<=";
    if (type == TOKEN_GT) return ">";
    if (type == TOKEN_GTEQ) return ">=";
    if (type == TOKEN_LOGICAL_OR) return "||";
    if (type == TOKEN_LOGICAL_AND) return "&&";
    if (type == TOKEN_LOGICAL_NOT) return "!";

    if (type == TOKEN_TYPE_ANY) return "any";
    if (type == TOKEN_NULL) return "null";
    if (type == TOKEN_TYPE_BOOL) return "bool";
    if (type == TOKEN_TRUE) return "true";
    if (type == TOKEN_FALSE) return "false";
    if (type == TOKEN_TYPE_INT) return "int";
    if (type == TOKEN_TYPE_FLOAT) return "float";
    if (type == TOKEN_TYPE_STRING) return "string";
    if (type == TOKEN_TYPE_ARRAY) return "array";
    if (type == TOKEN_TYPE_OBJECT) return "object";
    if (type == TOKEN_TYPE_FUNCTION) return "function";
    if (type == TOKEN_FUNCTION) return "fn";

    if (type == TOKEN_CONST) return "const";
    if (type == TOKEN_LET) return "let";
    if (type == TOKEN_IF) return "if";
    if (type == TOKEN_ELSE) return "else";
    if (type == TOKEN_WHILE) return "while";
    if (type == TOKEN_DO) return "do";
    if (type == TOKEN_FOR) return "for";
    if (type == TOKEN_IN) return "in";
    if (type == TOKEN_CONTINUE) return "continue";
    if (type == TOKEN_BREAK) return "break";
    if (type == TOKEN_RETURN) return "return";
    return NULL;
}

void token_free(Token *token) {
    token->refs--;
    if (token->refs > 0) return;

    if (token->type == TOKEN_KEYWORD || token->type == TOKEN_STRING) free(token->string);
    free(token);
}

int64_t string_to_int(char *string) {
    char *c = string;
    if (*c == '0' && *(c + 1) == 'b') {
        c += 2;
        return strtol(c, NULL, 2);
    }
    if (*c == '0' && isdigit(*(c + 1))) {
        c++;
        return strtol(c, NULL, 8);
    }
    if (*c == '0' && *(c + 1) == 'x') {
        c += 2;
        return strtol(c, NULL, 16);
    }
    return strtol(c, NULL, 10);
}

double string_to_float(char *string) { return strtod(string, NULL); }

List *lexer(char *text) {
    Keyword keywords[] = {{"any", TOKEN_TYPE_ANY},
                          {"null", TOKEN_NULL},
                          {"bool", TOKEN_TYPE_BOOL},
                          {"true", TOKEN_TRUE},
                          {"false", TOKEN_FALSE},
                          {"int", TOKEN_TYPE_INT},
                          {"float", TOKEN_TYPE_FLOAT},
                          {"string", TOKEN_TYPE_STRING},
                          {"array", TOKEN_TYPE_ARRAY},
                          {"object", TOKEN_TYPE_OBJECT},
                          {"function", TOKEN_TYPE_FUNCTION},
                          {"fn", TOKEN_FUNCTION},

                          {"const", TOKEN_CONST},
                          {"let", TOKEN_LET},
                          {"if", TOKEN_IF},
                          {"else", TOKEN_ELSE},
                          {"while", TOKEN_WHILE},
                          {"do", TOKEN_DO},
                          {"for", TOKEN_FOR},
                          {"in", TOKEN_IN},
                          {"continue", TOKEN_CONTINUE},
                          {"break", TOKEN_BREAK},
                          {"return", TOKEN_RETURN}};

    List *tokens = list_new_with_capacity(512);
    char *c = text;
    int32_t line = 0;
    char *lineStart = c;
    while (*c != '\0') {
        size_t position = c - lineStart;

        // Comments
        if (*c == '#') {
            while (*c != '\n' && *c != '\r') c++;
            continue;
        }
        if (*c == '/' && *(c + 1) == '/') {
            while (*c != '\n' && *c != '\r') c++;
            continue;
        }
        if (*c == '/' && *(c + 1) == '*') {
            c += 2;
            while (*c != '*' || *(c + 1) != '/') {
                if (*c == '\n' || *c == '\r') {
                    if (*c == '\r') c++;
                    c++;
                    line++;
                    lineStart = c;
                    continue;
                }
                c++;
            }
            c += 2;
            continue;
        }

        // Integers
        if (*c == '0' && *(c + 1) == 'b') {
            c += 2;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 2)));
            continue;
        }
        if (*c == '0' && isdigit(*(c + 1))) {
            c++;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 8)));
            continue;
        }
        if (*c == '0' && *(c + 1) == 'x') {
            c += 2;
            list_add(tokens, token_new_int(line, position, strtol(c, &c, 16)));
            continue;
        }
        if (isdigit(*c) || (*c == '.' && isdigit(*(c + 1)))) {
            double floating = strtod(c, &c);
            if ((double)((int64_t)floating) == floating) {
                list_add(tokens, token_new_int(line, position, (int64_t)floating));
            } else {
                list_add(tokens, token_new_float(line, position, floating));
            }
            continue;
        }

        // Strings
        if (*c == '"' || *c == '\'') {
            char endChar = *c;
            c++;
            char *ptr = c;
            while (*c != endChar) c++;
            size_t size = c - ptr;
            c++;

            char *string = malloc(size + 1);
            int32_t strpos = 0;
            for (size_t i = 0; i < size; i++) {
                if (ptr[i] == '\\') {
                    i++;
                    if (ptr[i] == 'b')
                        string[strpos++] = '\b';
                    else if (ptr[i] == 'f')
                        string[strpos++] = '\f';
                    else if (ptr[i] == 'n')
                        string[strpos++] = '\n';
                    else if (ptr[i] == 'r')
                        string[strpos++] = '\r';
                    else if (ptr[i] == 't')
                        string[strpos++] = '\t';
                    else if (ptr[i] == 'v')
                        string[strpos++] = '\v';
                    else if (ptr[i] == '\'')
                        string[strpos++] = '\'';
                    else if (ptr[i] == '"')
                        string[strpos++] = '"';
                    else if (ptr[i] == '\\')
                        string[strpos++] = '\\';
                    else
                        string[strpos++] = ptr[i];
                } else {
                    string[strpos++] = ptr[i];
                }
            }
            string[strpos] = '\0';
            list_add(tokens, token_new_string(TOKEN_STRING, line, position, string));
            continue;
        }

        // Keywords
        if (isalpha(*c) || *c == '_' || *c == '$') {
            char *ptr = c;
            while (isalnum(*c) || *c == '_' || *c == '$') c++;
            size_t size = c - ptr;

            bool found = false;
            for (size_t i = 0; i < sizeof(keywords) / sizeof(Keyword); i++) {
                Keyword *keyword = &keywords[i];
                size_t keywordSize = strlen(keyword->keyword);
                if (!memcmp(ptr, keyword->keyword, keywordSize) && size == keywordSize) {
                    list_add(tokens, token_new(keyword->type, line, position));
                    found = true;
                    break;
                }
            }
            if (!found) {
                char *string = malloc(size + 1);
                memcpy(string, ptr, size);
                string[size] = '\0';
                list_add(tokens, token_new_string(TOKEN_KEYWORD, line, position, string));
            }
            continue;
        }

        // Syntax
        if (*c == '(') {
            list_add(tokens, token_new(TOKEN_LPAREN, line, position));
            c++;
            continue;
        }
        if (*c == ')') {
            list_add(tokens, token_new(TOKEN_RPAREN, line, position));
            c++;
            continue;
        }
        if (*c == '{') {
            list_add(tokens, token_new(TOKEN_LCURLY, line, position));
            c++;
            continue;
        }
        if (*c == '}') {
            list_add(tokens, token_new(TOKEN_RCURLY, line, position));
            c++;
            continue;
        }
        if (*c == '[') {
            list_add(tokens, token_new(TOKEN_LBRACKET, line, position));
            c++;
            continue;
        }
        if (*c == ']') {
            list_add(tokens, token_new(TOKEN_RBRACKET, line, position));
            c++;
            continue;
        }
        if (*c == ';') {
            list_add(tokens, token_new(TOKEN_SEMICOLON, line, position));
            c++;
            continue;
        }
        if (*c == ':') {
            list_add(tokens, token_new(TOKEN_COLON, line, position));
            c++;
            continue;
        }
        if (*c == ',') {
            list_add(tokens, token_new(TOKEN_COMMA, line, position));
            c++;
            continue;
        }
        if (*c == '.') {
            list_add(tokens, token_new(TOKEN_POINT, line, position));
            c++;
            continue;
        }

        // Operators
        if (*c == '=') {
            if (*(c + 1) == '>') {
                list_add(tokens, token_new(TOKEN_FAT_ARROW, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_EQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_ASSIGN, line, position));
            c++;
            continue;
        }
        if (*c == '+') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_ADD, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_ADD, line, position));
            c++;
            continue;
        }
        if (*c == '-') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_SUB, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_SUB, line, position));
            c++;
            continue;
        }
        if (*c == '*') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_MUL, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '*') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_EXP, line, position));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_EXP, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MUL, line, position));
            c++;
            continue;
        }
        if (*c == '/') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_DIV, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_DIV, line, position));
            c++;
            continue;
        }
        if (*c == '%') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_MOD, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_MOD, line, position));
            c++;
            continue;
        }
        if (*c == '^') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_XOR, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_XOR, line, position));
            c++;
            continue;
        }
        if (*c == '~') {
            list_add(tokens, token_new(TOKEN_NOT, line, position));
            c++;
            continue;
        }
        if (*c == '<') {
            if (*(c + 1) == '<') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_SHL, line, position));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_SHL, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_LTEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LT, line, position));
            c++;
            continue;
        }
        if (*c == '>') {
            if (*(c + 1) == '>') {
                if (*(c + 2) == '=') {
                    list_add(tokens, token_new(TOKEN_ASSIGN_SHR, line, position));
                    c += 3;
                    continue;
                }
                list_add(tokens, token_new(TOKEN_SHR, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_GTEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_GT, line, position));
            c++;
            continue;
        }
        if (*c == '!') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_NEQ, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_LOGICAL_NOT, line, position));
            c++;
            continue;
        }
        if (*c == '|') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_OR, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '|') {
                list_add(tokens, token_new(TOKEN_LOGICAL_OR, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_OR, line, position));
            c++;
            continue;
        }
        if (*c == '&') {
            if (*(c + 1) == '=') {
                list_add(tokens, token_new(TOKEN_ASSIGN_AND, line, position));
                c += 2;
                continue;
            }
            if (*(c + 1) == '&') {
                list_add(tokens, token_new(TOKEN_LOGICAL_AND, line, position));
                c += 2;
                continue;
            }
            list_add(tokens, token_new(TOKEN_AND, line, position));
            c++;
            continue;
        }

        // Whitespace
        if (*c == ' ' || *c == '\t') {
            c++;
            continue;
        }
        if (*c == '\n' || *c == '\r') {
            if (*c == '\r') c++;
            c++;
            line++;
            lineStart = c;
            continue;
        }

        error(text, line, position, "Unexpected character: '%c'", *c);
    }
    list_add(tokens, token_new(TOKEN_EOF, line, c - lineStart));
    return tokens;
}

// Value
Argument *argument_new(char *name, ValueType type, Node *defaultNode) {
    Argument *argument = malloc(sizeof(Argument));
    argument->name = strdup(name);
    argument->type = type;
    argument->defaultNode = defaultNode;
    return argument;
}

void argument_free(Argument *argument) {
    free(argument->name);
    if (argument->defaultNode != NULL) {
        node_free(argument->defaultNode);
    }
    free(argument);
}

Value *value_new(ValueType type) {
    Value *value = malloc(sizeof(Value));
    value->refs = 1;
    value->type = type;
    return value;
}

Value *value_new_null(void) { return value_new(VALUE_NULL); }

Value *value_new_bool(bool boolean) {
    Value *value = value_new(VALUE_BOOL);
    value->boolean = boolean;
    return value;
}

Value *value_new_int(int64_t integer) {
    Value *value = value_new(VALUE_INT);
    value->integer = integer;
    return value;
}

Value *value_new_float(double floating) {
    Value *value = value_new(VALUE_FLOAT);
    value->floating = floating;
    return value;
}

Value *value_new_string(char *string) {
    Value *value = value_new(VALUE_STRING);
    value->string = strdup(string);
    return value;
}

Value *value_new_array(List *array) {
    Value *value = value_new(VALUE_ARRAY);
    value->array = array;
    return value;
}

Value *value_new_object(Map *object) {
    Value *value = value_new(VALUE_OBJECT);
    value->object = object;
    return value;
}

Value *value_new_function(List *args, ValueType returnType, Node *functionNode) {
    Value *value = value_new(VALUE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->functionNode = functionNode;
    return value;
}

Value *value_new_native_function(List *args, ValueType returnType, Value *(*nativeFunc)(List *values)) {
    Value *value = value_new(VALUE_NATIVE_FUNCTION);
    value->arguments = args;
    value->returnType = returnType;
    value->nativeFunc = nativeFunc;
    return value;
}

char *value_type_to_string(ValueType type) {
    if (type == VALUE_NULL) return "null";
    if (type == VALUE_BOOL) return "bool";
    if (type == VALUE_INT) return "int";
    if (type == VALUE_FLOAT) return "float";
    if (type == VALUE_STRING) return "string";
    if (type == VALUE_ARRAY) return "array";
    if (type == VALUE_OBJECT) return "object";
    if (type == VALUE_FUNCTION || type == VALUE_NATIVE_FUNCTION) return "function";
    return NULL;
}

char *value_to_string(Value *value) {
    if (value->type == VALUE_NULL) {
        return strdup("null");
    }
    if (value->type == VALUE_BOOL) {
        return strdup(value->boolean ? "true" : "false");
    }
    if (value->type == VALUE_INT) {
        char buffer[255];
        sprintf(buffer, "%lld", value->integer);
        return strdup(buffer);
    }
    if (value->type == VALUE_FLOAT) {
        char buffer[255];
        sprintf(buffer, "%g", value->floating);
        return strdup(buffer);
    }
    if (value->type == VALUE_STRING) {
        return strdup(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        return strdup("array");
    }
    if (value->type == VALUE_OBJECT) {
        return strdup("object");
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        return strdup("function");
    }
    return NULL;
}

ValueType token_type_to_value_type(TokenType type) {
    if (type == TOKEN_TYPE_ANY) return VALUE_ANY;
    if (type == TOKEN_NULL) return VALUE_NULL;
    if (type == TOKEN_TYPE_BOOL) return VALUE_BOOL;
    if (type == TOKEN_TYPE_INT) return VALUE_INT;
    if (type == TOKEN_TYPE_FLOAT) return VALUE_FLOAT;
    if (type == TOKEN_TYPE_STRING) return VALUE_STRING;
    if (type == TOKEN_TYPE_ARRAY) return VALUE_ARRAY;
    if (type == TOKEN_TYPE_OBJECT) return VALUE_OBJECT;
    if (type == TOKEN_TYPE_FUNCTION) return VALUE_FUNCTION;
    return 0;
}

Value *value_ref(Value *value) {
    value->refs++;
    return value;
}

Value *value_copy(Value *value) {
    if (value->type == VALUE_NULL) return value_new_null();
    if (value->type == VALUE_BOOL) return value_new_bool(value->boolean);
    if (value->type == VALUE_INT) return value_new_int(value->integer);
    if (value->type == VALUE_FLOAT) return value_new_float(value->floating);
    if (value->type == VALUE_STRING) return value_new_string(value->string);
    return NULL;
}

void value_clear(Value *value) {
    if (value->type == VALUE_STRING) {
        free(value->string);
    }
    if (value->type == VALUE_ARRAY) {
        list_free(value->array, (ListFreeFunc *)value_free);
    }
    if (value->type == VALUE_OBJECT) {
        map_free(value->object, (MapFreeFunc *)value_free);
    }
    if (value->type == VALUE_FUNCTION || value->type == VALUE_NATIVE_FUNCTION) {
        list_free(value->arguments, (ListFreeFunc *)argument_free);
    }
    if (value->type == VALUE_FUNCTION) {
        node_free(value->functionNode);
    }
}

void value_free(Value *value) {
    value->refs--;
    if (value->refs > 0) return;
    value_clear(value);
    free(value);
}

// Parser
Node *node_new(NodeType type, Token *token) {
    Node *node = malloc(sizeof(Node));
    node->refs = 1;
    node->type = type;
    if (token != NULL) {
        node->token = token_ref(token);
    }
    return node;
}

Node *node_new_value(Token *token, Value *value) {
    Node *node = node_new(NODE_VALUE, token);
    node->value = value;
    return node;
}

Node *node_new_string(NodeType type, Token *token, char *string) {
    Node *node = node_new(type, token);
    node->string = strdup(string);
    return node;
}

Node *node_new_unary(NodeType type, Token *token, Node *unary) {
    Node *node = node_new(type, token);
    node->unary = unary;
    return node;
}

Node *node_new_cast(Token *token, ValueType castType, Node *unary) {
    Node *node = node_new(NODE_CAST, token);
    node->castType = castType;
    node->unary = unary;
    return node;
}

Node *node_new_operation(NodeType type, Token *token, Node *lhs, Node *rhs) {
    Node *node = node_new(type, token);
    node->lhs = lhs;
    node->rhs = rhs;
    return node;
}

Node *node_new_multiple(NodeType type, Token *token) {
    Node *node = node_new(type, token);
    node->nodes = list_new();
    return node;
}

Node *node_ref(Node *node) {
    node->refs++;
    return node;
}

void node_free(Node *node) {
    node->refs--;
    if (node->refs > 0) return;

    if (node->token != NULL) {
        token_free(node->token);
    }
    if (node->type == NODE_VALUE) {
        value_free(node->value);
    }
    if (node->type == NODE_VARIABLE) {
        free(node->string);
    }
    if (node->type == NODE_RETURN || (node->type >= NODE_NEG && node->type <= NODE_CAST)) {
        node_free(node->unary);
    }
    if (node->type >= NODE_GET && node->type <= NODE_LOGICAL_OR) {
        node_free(node->lhs);
        node_free(node->rhs);
    }
    if (node->type >= NODE_IF && node->type <= NODE_FORIN) {
        node_free(node->condition);
        node_free(node->thenBlock);
        if (node->elseBlock != NULL) node_free(node->elseBlock);
    }
    if ((node->type >= NODE_PROGRAM && node->type <= NODE_BLOCK) || (node->type >= NODE_ARRAY && node->type <= NODE_CALL)) {
        if (node->type == NODE_CALL) node_free(node->function);
        if (node->type == NODE_OBJECT) list_free(node->keys, free);
        list_free(node->nodes, (ListFreeFunc *)node_free);
    }
    free(node);
}

Node *parser(char *text, List *tokens) {
    Parser parser = {.text = text, .tokens = tokens, .position = 0};
    return parser_program(&parser);
}

#define current() ((Token *)list_get(parser->tokens, parser->position))
#define next(pos) ((Token *)list_get(parser->tokens, parser->position + 1 + pos))

void parser_eat(Parser *parser, TokenType type) {
    if (current()->type == type) {
        parser->position++;
    } else {
        error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed '%s'", token_type_to_string(current()->type),
              token_type_to_string(type));
    }
}

Node *parser_program(Parser *parser) {
    Node *programNode = node_new_multiple(NODE_PROGRAM, current());
    while (current()->type != TOKEN_EOF) {
        Node *node = parser_statement(parser);
        if (node != NULL) list_add(programNode->nodes, node);
    }
    return programNode;
}

Node *parser_block(Parser *parser) {
    Node *blockNode = node_new_multiple(NODE_BLOCK, current());
    if (current()->type == TOKEN_LCURLY) {
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            Node *node = parser_statement(parser);
            if (node != NULL) list_add(blockNode->nodes, node);
        }
        parser_eat(parser, TOKEN_RCURLY);
    } else {
        Node *node = parser_statement(parser);
        if (node != NULL) list_add(blockNode->nodes, node);
    }
    return blockNode;
}

Node *parser_statement(Parser *parser) {
    if (current()->type == TOKEN_SEMICOLON) {
        parser_eat(parser, TOKEN_SEMICOLON);
        return NULL;
    }

    if (current()->type == TOKEN_LCURLY) {
        return parser_block(parser);
    }

    if (current()->type == TOKEN_IF) {
        Node *node = node_new(NODE_IF, current());
        parser_eat(parser, TOKEN_IF);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        node->thenBlock = parser_block(parser);
        if (current()->type == TOKEN_ELSE) {
            parser_eat(parser, TOKEN_ELSE);
            node->elseBlock = parser_block(parser);
        } else {
            node->elseBlock = NULL;
        }
        return node;
    }

    if (current()->type == TOKEN_WHILE) {
        Node *node = node_new(NODE_WHILE, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_WHILE);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        node->thenBlock = parser_block(parser);
        return node;
    }

    if (current()->type == TOKEN_DO) {
        Node *node = node_new(NODE_DOWHILE, current());
        node->elseBlock = NULL;
        parser_eat(parser, TOKEN_DO);
        node->thenBlock = parser_block(parser);
        parser_eat(parser, TOKEN_WHILE);
        parser_eat(parser, TOKEN_LPAREN);
        node->condition = parser_assigns(parser);
        parser_eat(parser, TOKEN_RPAREN);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FOR) {
        Token *token = current();
        parser_eat(parser, TOKEN_FOR);
        parser_eat(parser, TOKEN_LPAREN);
        Node *declarations;
        if (current()->type != TOKEN_SEMICOLON) {
            declarations = parser_declarations(parser);
        }

        if (current()->type == TOKEN_IN) {
            Node *node = node_new(NODE_FORIN, token);
            if (declarations->type == NODE_NODES) {
                error(parser->text, declarations->token->line, declarations->token->position, "You can only declare one variable in a for in loop");
            }
            node->variable = declarations;
            parser_eat(parser, TOKEN_IN);
            node->iterator = parser_logical(parser);
            parser_eat(parser, TOKEN_RPAREN);
            node->thenBlock = parser_block(parser);
            return node;
        }

        Node *blockNode = node_new_multiple(NODE_BLOCK, token);
        list_add(blockNode->nodes, declarations);

        Node *node = node_new(NODE_FOR, token);
        parser_eat(parser, TOKEN_SEMICOLON);
        if (current()->type != TOKEN_SEMICOLON) {
            node->condition = parser_logical(parser);
        } else {
            node->condition = NULL;
        }
        parser_eat(parser, TOKEN_SEMICOLON);

        if (current()->type != TOKEN_RPAREN) {
            node->incrementBlock = parser_assigns(parser);
        } else {
            node->incrementBlock = NULL;
        }
        parser_eat(parser, TOKEN_RPAREN);

        node->thenBlock = parser_block(parser);
        list_add(blockNode->nodes, node);
        return blockNode;
    }

    if (current()->type == TOKEN_CONTINUE) {
        Token *token = current();
        parser_eat(parser, TOKEN_CONTINUE);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_CONTINUE, token);
    }
    if (current()->type == TOKEN_BREAK) {
        Token *token = current();
        parser_eat(parser, TOKEN_BREAK);
        parser_eat(parser, TOKEN_SEMICOLON);
        return node_new(NODE_BREAK, token);
    }
    if (current()->type == TOKEN_RETURN) {
        Token *token = current();
        parser_eat(parser, TOKEN_RETURN);
        Node *node = node_new_unary(NODE_RETURN, token, parser_logical(parser));
        parser_eat(parser, TOKEN_SEMICOLON);
        return node;
    }

    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        Token *nameToken = current();
        char *name = current()->string;
        parser_eat(parser, TOKEN_KEYWORD);

        List *arguments = list_new();
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(arguments, parser_argument(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);

        ValueType returnType = VALUE_ANY;
        if (current()->type == TOKEN_COLON) {
            parser_eat(parser, TOKEN_COLON);
            if (token_type_is_type(current()->type)) {
                returnType = token_type_to_value_type(current()->type);
                parser_eat(parser, current()->type);
            } else {
                error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
            }
        }

        Value *functionValue;
        if (current()->type == TOKEN_FAT_ARROW) {
            Token *token = current();
            parser_eat(parser, TOKEN_FAT_ARROW);
            functionValue = value_new_function(arguments, returnType, node_new_unary(NODE_RETURN, token, parser_logical(parser)));
        } else {
            functionValue = value_new_function(arguments, returnType, parser_block(parser));
        }
        Node *node =
            node_new_operation(NODE_CONST_ASSIGN, functionToken, node_new_string(NODE_VARIABLE, nameToken, name), node_new_value(functionToken, functionValue));
        node->declarationType = VALUE_FUNCTION;
        return node;
    }

    Node *node = parser_declarations(parser);
    parser_eat(parser, TOKEN_SEMICOLON);
    return node;
}

Node *parser_declarations(Parser *parser) {
    if (current()->type == TOKEN_CONST || current()->type == TOKEN_LET) {
        List *nodes = list_new();
        NodeType assignType;
        if (current()->type == TOKEN_CONST) {
            assignType = NODE_CONST_ASSIGN;
            parser_eat(parser, TOKEN_CONST);
        }
        if (current()->type == TOKEN_LET) {
            assignType = NODE_LET_ASSIGN;
            parser_eat(parser, TOKEN_LET);
        }

        for (;;) {
            Node *variable = node_new_string(NODE_VARIABLE, current(), current()->string);
            parser_eat(parser, TOKEN_KEYWORD);

            ValueType declarationType = VALUE_ANY;
            if (current()->type == TOKEN_COLON) {
                parser_eat(parser, TOKEN_COLON);
                if (token_type_is_type(current()->type)) {
                    declarationType = token_type_to_value_type(current()->type);
                    parser_eat(parser, current()->type);
                } else {
                    error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token",
                          token_type_to_string(current()->type));
                }
            }

            Node *node;
            if (current()->type == TOKEN_ASSIGN) {
                Token *token = current();
                parser_eat(parser, TOKEN_ASSIGN);
                node = node_new_operation(assignType, token, variable, parser_assign(parser));
            } else {
                node = node_new_operation(assignType, variable->token, variable, node_new_value(variable->token, value_new_null()));
            }
            node->declarationType = declarationType;
            list_add(nodes, node);

            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        if (nodes->size == 0) {
            list_free(nodes, NULL);
            return NULL;
        }
        if (nodes->size == 1) {
            Node *firstNode = list_get(nodes, 0);
            list_free(nodes, NULL);
            return firstNode;
        }
        Node *nodesNode = node_new(NODE_NODES, current());
        nodesNode->nodes = nodes;
        return nodesNode;
    }
    return parser_assigns(parser);
}

Node *parser_assigns(Parser *parser) {
    List *nodes = list_new();
    for (;;) {
        Node *node = parser_assign(parser);
        list_add(nodes, node);
        if (current()->type == TOKEN_COMMA) {
            parser_eat(parser, TOKEN_COMMA);
        } else {
            break;
        }
    }
    if (nodes->size == 0) {
        list_free(nodes, NULL);
        return NULL;
    }
    if (nodes->size == 1) {
        Node *firstNode = list_get(nodes, 0);
        list_free(nodes, NULL);
        return firstNode;
    }
    Node *nodesNode = node_new(NODE_NODES, current());
    nodesNode->nodes = nodes;
    return nodesNode;
}

Node *parser_assign(Parser *parser) {
    Node *lhs = parser_logical(parser);  // TODO
    if (current()->type == TOKEN_ASSIGN) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN);
        return node_new_operation(NODE_ASSIGN, token, lhs, parser_assign(parser));
    }
    if (current()->type == TOKEN_ASSIGN_ADD) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_ADD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_ADD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SUB) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SUB);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SUB, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MUL) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MUL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MUL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_EXP) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_EXP);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_EXP, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_MOD) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_MOD);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_MOD, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_AND) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_AND);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_AND, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_XOR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_XOR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_XOR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_OR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_OR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_OR, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHL) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHL);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHL, token, node_ref(lhs), parser_assign(parser)));
    }
    if (current()->type == TOKEN_ASSIGN_SHR) {
        Token *token = current();
        parser_eat(parser, TOKEN_ASSIGN_SHR);
        return node_new_operation(NODE_ASSIGN, token, lhs, node_new_operation(NODE_SHR, token, node_ref(lhs), parser_assign(parser)));
    }
    return lhs;
}

Node *parser_logical(Parser *parser) {
    Node *node = parser_bitwise(parser);
    while (current()->type == TOKEN_LOGICAL_AND || current()->type == TOKEN_LOGICAL_OR) {
        if (current()->type == TOKEN_LOGICAL_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_AND);
            node = node_new_operation(NODE_LOGICAL_AND, token, node, parser_bitwise(parser));
        }
        if (current()->type == TOKEN_LOGICAL_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_LOGICAL_OR);
            node = node_new_operation(NODE_LOGICAL_OR, token, node, parser_bitwise(parser));
        }
    }
    return node;
}

Node *parser_bitwise(Parser *parser) {
    Node *node = parser_equality(parser);
    while (current()->type == TOKEN_AND || current()->type == TOKEN_XOR || current()->type == TOKEN_OR) {
        if (current()->type == TOKEN_AND) {
            Token *token = current();
            parser_eat(parser, TOKEN_AND);
            node = node_new_operation(NODE_AND, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_XOR) {
            Token *token = current();
            parser_eat(parser, TOKEN_XOR);
            node = node_new_operation(NODE_XOR, token, node, parser_equality(parser));
        }
        if (current()->type == TOKEN_OR) {
            Token *token = current();
            parser_eat(parser, TOKEN_OR);
            node = node_new_operation(NODE_OR, token, node, parser_equality(parser));
        }
    }
    return node;
}

Node *parser_equality(Parser *parser) {
    Node *node = parser_relational(parser);
    while (current()->type == TOKEN_EQ || current()->type == TOKEN_NEQ) {
        if (current()->type == TOKEN_EQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_EQ);
            node = node_new_operation(NODE_EQ, token, node, parser_relational(parser));
        }
        if (current()->type == TOKEN_NEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_NEQ);
            node = node_new_operation(NODE_NEQ, token, node, parser_relational(parser));
        }
    }
    return node;
}

Node *parser_relational(Parser *parser) {
    Node *node = parser_shift(parser);
    while (current()->type == TOKEN_LT || current()->type == TOKEN_LTEQ || current()->type == TOKEN_GT || current()->type == TOKEN_GTEQ) {
        if (current()->type == TOKEN_LT) {
            Token *token = current();
            parser_eat(parser, TOKEN_LT);
            node = node_new_operation(NODE_LT, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_LTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_LTEQ);
            node = node_new_operation(NODE_LTEQ, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_GT) {
            Token *token = current();
            parser_eat(parser, TOKEN_GT);
            node = node_new_operation(NODE_GT, token, node, parser_shift(parser));
        }
        if (current()->type == TOKEN_GTEQ) {
            Token *token = current();
            parser_eat(parser, TOKEN_GTEQ);
            node = node_new_operation(NODE_GTEQ, token, node, parser_shift(parser));
        }
    }
    return node;
}

Node *parser_shift(Parser *parser) {
    Node *node = parser_add(parser);
    while (current()->type == TOKEN_SHL || current()->type == TOKEN_SHR) {
        if (current()->type == TOKEN_SHL) {
            Token *token = current();
            parser_eat(parser, TOKEN_SHL);
            node = node_new_operation(NODE_SHL, token, node, parser_add(parser));
        }

        if (current()->type == TOKEN_SHR) {
            Token *token = current();
            parser_eat(parser, TOKEN_SHR);
            node = node_new_operation(NODE_SHR, token, node, parser_add(parser));
        }
    }
    return node;
}

Node *parser_add(Parser *parser) {
    Node *node = parser_mul(parser);
    while (current()->type == TOKEN_ADD || current()->type == TOKEN_SUB) {
        if (current()->type == TOKEN_ADD) {
            Token *token = current();
            parser_eat(parser, TOKEN_ADD);
            node = node_new_operation(NODE_ADD, token, node, parser_mul(parser));
        }

        if (current()->type == TOKEN_SUB) {
            Token *token = current();
            parser_eat(parser, TOKEN_SUB);
            node = node_new_operation(NODE_SUB, token, node, parser_mul(parser));
        }
    }
    return node;
}

Node *parser_mul(Parser *parser) {
    Node *node = parser_unary(parser);
    while (current()->type == TOKEN_MUL || current()->type == TOKEN_EXP || current()->type == TOKEN_DIV || current()->type == TOKEN_MOD) {
        if (current()->type == TOKEN_MUL) {
            Token *token = current();
            parser_eat(parser, TOKEN_MUL);
            node = node_new_operation(NODE_MUL, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_EXP) {
            Token *token = current();
            parser_eat(parser, TOKEN_EXP);
            node = node_new_operation(NODE_EXP, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_DIV) {
            Token *token = current();
            parser_eat(parser, TOKEN_DIV);
            node = node_new_operation(NODE_DIV, token, node, parser_unary(parser));
        }
        if (current()->type == TOKEN_MOD) {
            Token *token = current();
            parser_eat(parser, TOKEN_MOD);
            node = node_new_operation(NODE_MOD, token, node, parser_unary(parser));
        }
    }
    return node;
}

Node *parser_unary(Parser *parser) {
    if (current()->type == TOKEN_ADD) {
        parser_eat(parser, TOKEN_ADD);
        return parser_unary(parser);
    }
    if (current()->type == TOKEN_SUB) {
        Token *token = current();
        parser_eat(parser, TOKEN_SUB);
        return node_new_unary(NODE_NEG, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_NOT) {
        Token *token = current();
        parser_eat(parser, TOKEN_NOT);
        return node_new_unary(NODE_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LOGICAL_NOT) {
        Token *token = current();
        parser_eat(parser, TOKEN_LOGICAL_NOT);
        return node_new_unary(NODE_LOGICAL_NOT, token, parser_unary(parser));
    }
    if (current()->type == TOKEN_LPAREN && token_type_is_type(next(0)->type) && next(1)->type == TOKEN_RPAREN) {
        Token *token = current();
        parser_eat(parser, TOKEN_LPAREN);
        if (token_type_is_type(current()->type)) {
            ValueType castType = token_type_to_value_type(current()->type);
            parser_eat(parser, current()->type);
            parser_eat(parser, TOKEN_RPAREN);
            return node_new_cast(token, castType, parser_unary(parser));
        }
        error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
    }
    return parser_primary(parser);
}

Node *parser_primary(Parser *parser) {
    if (current()->type == TOKEN_LPAREN) {
        parser_eat(parser, TOKEN_LPAREN);
        Node *node = parser_logical(parser);
        parser_eat(parser, TOKEN_RPAREN);
        return node;
    }
    if (current()->type == TOKEN_NULL) {
        Node *node = node_new_value(current(), value_new(VALUE_NULL));
        parser_eat(parser, TOKEN_NULL);
        return node;
    }
    if (current()->type == TOKEN_TRUE) {
        Node *node = node_new_value(current(), value_new_bool(true));
        parser_eat(parser, TOKEN_TRUE);
        return node;
    }
    if (current()->type == TOKEN_FALSE) {
        Node *node = node_new_value(current(), value_new_bool(false));
        parser_eat(parser, TOKEN_FALSE);
        return node;
    }
    if (current()->type == TOKEN_INT) {
        Node *node = node_new_value(current(), value_new_int(current()->integer));
        parser_eat(parser, TOKEN_INT);
        return node;
    }
    if (current()->type == TOKEN_FLOAT) {
        Node *node = node_new_value(current(), value_new_float(current()->floating));
        parser_eat(parser, TOKEN_FLOAT);
        return node;
    }
    if (current()->type == TOKEN_STRING) {
        Node *node = node_new_value(current(), value_new_string(current()->string));
        parser_eat(parser, TOKEN_STRING);
        return node;
    }
    if (current()->type == TOKEN_LBRACKET) {
        Node *node = node_new_multiple(NODE_ARRAY, current());
        parser_eat(parser, TOKEN_LBRACKET);
        while (current()->type != TOKEN_RBRACKET) {
            list_add(node->nodes, parser_assign(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RBRACKET);
        return node;
    }
    if (current()->type == TOKEN_LCURLY) {
        Node *node = node_new_multiple(NODE_OBJECT, current());
        node->keys = list_new_with_capacity(node->nodes->capacity);
        parser_eat(parser, TOKEN_LCURLY);
        while (current()->type != TOKEN_RCURLY) {
            list_add(node->keys, strdup(current()->string));
            parser_eat(parser, TOKEN_KEYWORD);
            parser_eat(parser, TOKEN_ASSIGN);
            list_add(node->nodes, parser_logical(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RCURLY);
        return node;
    }
    if (current()->type == TOKEN_FUNCTION) {
        Token *functionToken = current();
        parser_eat(parser, TOKEN_FUNCTION);
        List *arguments = list_new();
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(arguments, parser_argument(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);

        ValueType returnType = VALUE_ANY;
        if (current()->type == TOKEN_COLON) {
            parser_eat(parser, TOKEN_COLON);
            if (token_type_is_type(current()->type)) {
                returnType = token_type_to_value_type(current()->type);
                parser_eat(parser, current()->type);
            } else {
                error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
            }
        }

        if (current()->type == TOKEN_FAT_ARROW) {
            Token *token = current();
            parser_eat(parser, TOKEN_FAT_ARROW);
            Node *returnNode = node_new_unary(NODE_RETURN, token, parser_logical(parser));
            return node_new_value(functionToken, value_new_function(arguments, returnType, returnNode));
        }
        return node_new_value(functionToken, value_new_function(arguments, returnType, parser_block(parser)));
    }

    return parser_identifier(parser);
}

Node *parser_identifier(Parser *parser) {
    if (current()->type == TOKEN_KEYWORD) {
        Node *node = node_new_string(NODE_VARIABLE, current(), current()->string);
        parser_eat(parser, TOKEN_KEYWORD);
        return parser_identifier_suffix(parser, node);
    }

    error(parser->text, current()->line, current()->position, "Unexpected token: '%s'", token_type_to_string(current()->type));
    return NULL;
}

Node *parser_identifier_suffix(Parser *parser, Node *node) {
    while (current()->type == TOKEN_LBRACKET || current()->type == TOKEN_POINT) {
        Token *token = current();
        if (current()->type == TOKEN_LBRACKET) {
            parser_eat(parser, TOKEN_LBRACKET);
            Node *indexOrKey = parser_assign(parser);
            parser_eat(parser, TOKEN_RBRACKET);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
        if (current()->type == TOKEN_POINT) {
            parser_eat(parser, TOKEN_POINT);
            Node *indexOrKey = node_new_value(current(), value_new_string(current()->string));
            parser_eat(parser, TOKEN_KEYWORD);
            node = node_new_operation(NODE_GET, token, node, indexOrKey);
        }
    }
    if (current()->type == TOKEN_LPAREN) {
        Node *callNode = node_new_multiple(NODE_CALL, current());
        callNode->function = node;
        parser_eat(parser, TOKEN_LPAREN);
        while (current()->type != TOKEN_RPAREN) {
            list_add(callNode->nodes, parser_assign(parser));
            if (current()->type == TOKEN_COMMA) {
                parser_eat(parser, TOKEN_COMMA);
            } else {
                break;
            }
        }
        parser_eat(parser, TOKEN_RPAREN);
        return callNode;
    }
    return node;
}

Argument *parser_argument(Parser *parser) {
    char *name = current()->string;
    parser_eat(parser, TOKEN_KEYWORD);
    ValueType type = VALUE_ANY;
    if (current()->type == TOKEN_COLON) {
        parser_eat(parser, TOKEN_COLON);
        if (!token_type_is_type(current()->type)) {
            error(parser->text, current()->line, current()->position, "Unexpected token: '%s' needed type token", token_type_to_string(current()->type));
        }
        type = token_type_to_value_type(current()->type);
        parser_eat(parser, current()->type);
    }
    Node *defaultNode = NULL;
    if (current()->type == TOKEN_ASSIGN) {
        parser_eat(parser, TOKEN_ASSIGN);
        defaultNode = parser_logical(parser);
    }
    return argument_new(name, type, defaultNode);
}

// Standard library header
Value *env_type(List *values);
Value *env_print(List *values);
Value *env_println(List *values);
Value *env_exit(List *values);
Value *env_array_length(List *values);
Value *env_array_push(List *values);
Value *env_string_length(List *values);

// Interpreter
Variable *variable_new(ValueType type, bool mutable, Value *value) {
    Variable *variable = malloc(sizeof(Variable));
    variable->type = type;
    variable->mutable = mutable;
    variable->value = value;
    return variable;
}

void variable_free(Variable *variable) {
    value_free(variable->value);
    free(variable);
}

Value *interpreter(char *text, Map *env, Node *node) {
    Interpreter interpreter = {.text = text, .env = env};
    Scope scope = {.function = &(FunctionScope){.returnValue = NULL},
                   .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                   .block = &(BlockScope){.parentBlock = NULL, .env = env}};
    return interpreter_node(&interpreter, &scope, node);
}

Variable *block_scope_get(BlockScope *block, char *key) {
    Variable *variable = map_get(block->env, key);
    if (variable == NULL && block->parentBlock != NULL) {
        return block_scope_get(block->parentBlock, key);
    }
    return variable;
}

#define interpreter_statement_in_loop(interpreter, scope, node, cleanup) \
    {                                                                    \
        Value *nodeValue = interpreter_node(interpreter, scope, node);   \
        if (nodeValue != NULL) value_free(nodeValue);                    \
        if ((scope)->function->returnValue != NULL) {                    \
            cleanup;                                                     \
            return NULL;                                                 \
        }                                                                \
    }

#define interpreter_statement(interpreter, scope, node, cleanup)      \
    interpreter_statement_in_loop(interpreter, scope, node, cleanup); \
    if ((scope)->loop->inLoop) {                                      \
        if ((scope)->loop->isContinuing) {                            \
            cleanup;                                                  \
            return NULL;                                              \
        }                                                             \
        if ((scope)->loop->isBreaking) {                              \
            cleanup;                                                  \
            return NULL;                                              \
        }                                                             \
    }

Value *interpreter_node(Interpreter *interpreter, Scope *scope, Node *node) {
    if (node->type == NODE_PROGRAM) {
        list_foreach(node->nodes, Node * child, { interpreter_statement(interpreter, scope, child, {}); });
        return value_new_null();
    }
    if (node->type == NODE_NODES) {
        list_foreach(node->nodes, Node * child, { interpreter_statement(interpreter, scope, child, {}); });
        return NULL;
    }
    if (node->type == NODE_BLOCK) {
        Scope blockScope = {.function = scope->function, .loop = scope->loop, .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
        list_foreach(node->nodes, Node * child,
                     { interpreter_statement(interpreter, &blockScope, child, { map_free(blockScope.block->env, (MapFreeFunc *)variable_free); }); });
        map_free(blockScope.block->env, (MapFreeFunc *)variable_free);
        return NULL;
    }
    if (node->type == NODE_IF) {
        Value *condition = interpreter_node(interpreter, scope, node->condition);
        if (condition->type != VALUE_BOOL) {
            error(interpreter->text, node->token->line, node->token->position, "If condition type is not a bool");
        }
        if (condition->boolean) {
            interpreter_statement(interpreter, scope, node->thenBlock, { value_free(condition); });
        } else if (node->elseBlock != NULL) {
            interpreter_statement(interpreter, scope, node->elseBlock, { value_free(condition); });
        }
        value_free(condition);
        return NULL;
    }
    if (node->type == NODE_WHILE) {
        Scope loopScope = {
            .function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                error(interpreter->text, node->token->line, node->token->position, "While condition type is not a bool");
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        return NULL;
    }
    if (node->type == NODE_DOWHILE) {
        Scope loopScope = {
            .function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }

            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                error(interpreter->text, node->token->line, node->token->position, "While condition type is not a bool");
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);
        }
        return NULL;
    }
    if (node->type == NODE_FOR) {
        Scope loopScope = {
            .function = scope->function, .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false}, .block = scope->block};
        for (;;) {
            Value *condition = interpreter_node(interpreter, scope, node->condition);
            if (condition->type != VALUE_BOOL) {
                error(interpreter->text, node->token->line, node->token->position, "For condition type is not a bool");
            }
            if (!condition->boolean) {
                value_free(condition);
                break;
            }
            value_free(condition);

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }

            interpreter_statement(interpreter, &loopScope, node->incrementBlock, {});
        }
        return NULL;
    }
    if (node->type == NODE_FORIN) {
        Value *iterator = interpreter_node(interpreter, scope, node->iterator);
        if (iterator->type != VALUE_STRING && iterator->type != VALUE_ARRAY && iterator->type != VALUE_OBJECT) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a string, array or object it is: %s",
                  value_type_to_string(iterator->type));
        }

        Scope loopScope = {.function = scope->function,
                           .loop = &(LoopScope){.inLoop = true, .isContinuing = false, .isBreaking = false},
                           .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
        size_t size;
        if (iterator->type == VALUE_STRING) {
            size = strlen(iterator->string);
        }
        if (iterator->type == VALUE_ARRAY) {
            size = iterator->array->size;
        }
        if (iterator->type == VALUE_OBJECT) {
            size = iterator->object->size;
        }

        for (size_t i = 0; i < size; i++) {
            Value *iteratorValue;
            if (iterator->type == VALUE_STRING) {
                char character[] = {iterator->string[i], '\0'};
                iteratorValue = value_new_string(character);
            }
            if (iterator->type == VALUE_ARRAY) {
                iteratorValue = value_ref(list_get(iterator->array, i));
            }
            if (iterator->type == VALUE_OBJECT) {
                iteratorValue = value_new_string(iterator->object->keys[i]);
            }
            Variable *previousVariable = map_get(loopScope.block->env, node->variable->lhs->string);
            if (previousVariable != NULL) {
                value_free(previousVariable->value);
                previousVariable->value = iteratorValue;
            } else {
                map_set(loopScope.block->env, node->variable->lhs->string,
                        variable_new(node->variable->declarationType, node->variable->type == NODE_LET_ASSIGN, iteratorValue));
            }

            interpreter_statement_in_loop(interpreter, &loopScope, node->thenBlock, {});
            if (loopScope.loop->isContinuing) {
                loopScope.loop->isContinuing = false;
            }
            if (loopScope.loop->isBreaking) {
                break;
            }
        }
        map_free(loopScope.block->env, (MapFreeFunc *)variable_free);
        value_free(iterator);
        return NULL;
    }
    if (node->type == NODE_CONTINUE) {
        if (!scope->loop->inLoop) {
            error(interpreter->text, node->token->line, node->token->position, "Not in a loop");
        }
        scope->loop->isContinuing = true;
        return NULL;
    }
    if (node->type == NODE_BREAK) {
        if (!scope->loop->inLoop) {
            error(interpreter->text, node->token->line, node->token->position, "Not in a loop");
        }
        scope->loop->isBreaking = true;
        return NULL;
    }
    if (node->type == NODE_RETURN) {
        scope->function->returnValue = interpreter_node(interpreter, scope, node->unary);
        return NULL;
    }

    if (node->type == NODE_VALUE) {
        return value_ref(node->value);
    }
    if (node->type == NODE_ARRAY) {
        Value *arrayValue = value_new_array(list_new_with_capacity(align(node->nodes->size, 8)));
        list_foreach(node->nodes, Node * item, { list_add(arrayValue->array, interpreter_node(interpreter, scope, item)); });
        return arrayValue;
    }
    if (node->type == NODE_OBJECT) {
        Value *objectValue = value_new_object(map_new_with_capacity(align(node->nodes->size, 8)));
        for (size_t i = 0; i < node->nodes->size; i++) {
            map_set(objectValue->object, list_get(node->keys, i), interpreter_node(interpreter, scope, list_get(node->nodes, i)));
        }
        return objectValue;
    }
    if (node->type == NODE_CALL) {
        Value *functionValue = interpreter_node(interpreter, scope, node->function);
        if (functionValue->type != VALUE_FUNCTION && functionValue->type != VALUE_NATIVE_FUNCTION) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a function");
        }

        List *values = list_new_with_capacity(align(node->nodes->size, 8));
        for (size_t i = 0; i < MAX(functionValue->arguments->size, node->nodes->size); i++) {
            Argument *argument = list_get(functionValue->arguments, i);
            if (list_get(node->nodes, i) == NULL && argument != NULL) {
                if (argument->defaultNode != NULL) {
                    Value *defaultValue = interpreter_node(interpreter, scope, argument->defaultNode);
                    if (argument->type != VALUE_ANY && defaultValue->type != argument->type) {
                        error(interpreter->text, argument->defaultNode->token->line, argument->defaultNode->token->position,
                              "Unexpected function default argument type: '%s' needed '%s'", value_type_to_string(defaultValue->type),
                              value_type_to_string(argument->type));
                    }
                    list_add(values, defaultValue);
                    continue;
                }

                error(interpreter->text, node->token->line, node->token->position, "Not all function arguments are given");
            }

            Value *nodeValue = interpreter_node(interpreter, scope, list_get(node->nodes, i));
            if (argument != NULL && argument->type != VALUE_ANY && nodeValue->type != argument->type) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function argument type: '%s' needed '%s'",
                      value_type_to_string(nodeValue->type), value_type_to_string(argument->type));
            }
            list_add(values, nodeValue);
        }

        Value *returnValue = NULL;
        if (functionValue->type == VALUE_FUNCTION) {
            Scope functionScope = {.function = &(FunctionScope){.returnValue = NULL},
                                   .loop = &(LoopScope){.inLoop = false, .isContinuing = false, .isBreaking = false},
                                   .block = &(BlockScope){.parentBlock = scope->block, .env = map_new()}};
            for (size_t i = 0; i < functionValue->arguments->size; i++) {
                Argument *argument = list_get(functionValue->arguments, i);
                map_set(functionScope.block->env, argument->name, variable_new(argument->type, true, value_ref(list_get(values, i))));
            }
            map_set(functionScope.block->env, "arguments", variable_new(VALUE_ARRAY, false, value_new_array(list_ref(values))));
            interpreter_node(interpreter, &functionScope, functionValue->functionNode);

            map_free(functionScope.block->env, (MapFreeFunc *)variable_free);

            if (functionValue->returnType != VALUE_ANY && functionScope.function->returnValue->type != functionValue->returnType) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function return type: '%s' needed '%s'",
                      value_type_to_string(functionScope.function->returnValue->type), value_type_to_string(functionValue->returnType));
            }
            if (functionScope.function->returnValue != NULL) {
                returnValue = functionScope.function->returnValue;
            } else {
                returnValue = value_new_null();
            }
        }

        if (functionValue->type == VALUE_NATIVE_FUNCTION) {
            returnValue = functionValue->nativeFunc(values);
            if (functionValue->returnType != VALUE_ANY && returnValue->type != functionValue->returnType) {
                error(interpreter->text, node->token->line, node->token->position, "Unexpected function return type: '%s' needed '%s'",
                      value_type_to_string(returnValue->type), value_type_to_string(functionValue->returnType));
            }
        }

        list_free(values, (ListFreeFunc *)value_free);
        value_free(functionValue);
        return returnValue;
    }

    if (node->type >= NODE_NEG && node->type <= NODE_CAST) {
        Value *unary = interpreter_node(interpreter, scope, node->unary);
        if (node->type == NODE_NEG) {
            if (unary->type == VALUE_INT) {
                unary->integer = -unary->integer;
                return unary;
            }
            if (unary->type == VALUE_FLOAT) {
                unary->floating = -unary->floating;
                return unary;
            }
        }
        if (node->type == NODE_NOT) {
            if (unary->type == VALUE_INT) {
                unary->integer = ~unary->integer;
                return unary;
            }
        }
        if (node->type == NODE_LOGICAL_NOT) {
            if (unary->type == VALUE_BOOL) {
                unary->boolean = !unary->boolean;
                return unary;
            }
        }
        if (node->type == NODE_CAST) {
            if (node->castType == VALUE_BOOL) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = false;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) return unary;
                if (unary->type == VALUE_INT) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = unary->integer != 0;
                    return unary;
                }
                if (unary->type == VALUE_FLOAT) {
                    unary->type = VALUE_BOOL;
                    unary->boolean = unary->floating != 0.0;
                    return unary;
                }
                if (unary->type == VALUE_STRING) {
                    bool result = !(!strcmp(unary->string, "") || !strcmp(unary->string, "0"));
                    value_clear(unary);
                    unary->type = VALUE_BOOL;
                    unary->boolean = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_INT) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_INT;
                    unary->integer = 0;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) {
                    unary->type = VALUE_INT;
                    unary->integer = unary->boolean;
                    return unary;
                }
                if (unary->type == VALUE_INT) return unary;
                if (unary->type == VALUE_FLOAT) {
                    unary->type = VALUE_INT;
                    unary->integer = unary->floating;
                    return unary;
                }
                if (unary->type == VALUE_STRING) {
                    int64_t result = string_to_int(unary->string);
                    value_clear(unary);
                    unary->type = VALUE_INT;
                    unary->integer = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_FLOAT) {
                if (unary->type == VALUE_NULL) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = 0;
                    return unary;
                }
                if (unary->type == VALUE_BOOL) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = unary->boolean;
                    return unary;
                }
                if (unary->type == VALUE_INT) {
                    unary->type = VALUE_FLOAT;
                    unary->floating = unary->integer;
                    return unary;
                }
                if (unary->type == VALUE_FLOAT) return unary;
                if (unary->type == VALUE_STRING) {
                    int64_t result = string_to_float(unary->string);
                    value_clear(unary);
                    unary->type = VALUE_FLOAT;
                    unary->floating = result;
                    return unary;
                }
            }

            if (node->castType == VALUE_STRING) {
                char *string = value_to_string(unary);
                value_clear(unary);
                unary->type = VALUE_STRING;
                unary->string = string;
                return unary;
            }
        }

        error(interpreter->text, node->token->line, node->token->position, "Type error");
    }

    if (node->type == NODE_VARIABLE) {
        Variable *variable = block_scope_get(scope->block, node->string);
        if (variable == NULL) {
            error(interpreter->text, node->token->line, node->token->position, "Can't find variable: '%s'", node->string);
        }
        if (variable->value->type == VALUE_NULL || variable->value->type == VALUE_BOOL || variable->value->type == VALUE_INT ||
            variable->value->type == VALUE_FLOAT || variable->value->type == VALUE_STRING) {
            return value_copy(variable->value);
        } else {
            return value_ref(variable->value);
        }
    }
    if (node->type == NODE_GET) {
        Value *containerValue = interpreter_node(interpreter, scope, node->lhs);
        if (containerValue->type != VALUE_STRING && containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT) {
            error(interpreter->text, node->token->line, node->token->position, "Variable is not a string, array or object it is: %s",
                  value_type_to_string(containerValue->type));
        }

        Value *indexOrKey = interpreter_node(interpreter, scope, node->rhs);
        Value *returnValue;
        if (containerValue->type == VALUE_STRING) {
            if (indexOrKey->type != VALUE_INT) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "String index is not an int");
            }
            if (indexOrKey->integer >= 0 && indexOrKey->integer <= (int64_t)strlen(containerValue->string)) {
                char character[] = {containerValue->string[indexOrKey->integer], '\0'};
                returnValue = value_new_string(character);
            } else {
                returnValue = value_new_null();
            }
        }
        if (containerValue->type == VALUE_ARRAY) {
            if (indexOrKey->type != VALUE_INT) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Array index is not an int");
            }
            Value *value = list_get(containerValue->array, indexOrKey->integer);
            if (value != NULL) {
                if (value->type == VALUE_NULL || value->type == VALUE_BOOL || value->type == VALUE_INT || value->type == VALUE_FLOAT ||
                    value->type == VALUE_STRING) {
                    returnValue = value_copy(value);
                } else {
                    returnValue = value_ref(value);
                }
            } else {
                returnValue = value_new_null();
            }
        }
        if (containerValue->type == VALUE_OBJECT) {
            if (indexOrKey->type != VALUE_STRING) {
                error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Object key is not a string");
            }
            Value *value = map_get(containerValue->object, indexOrKey->string);
            if (value == NULL) {
                error(interpreter->text, node->token->line, node->token->position, "Can't find key in object");
            }
            if (value->type == VALUE_NULL || value->type == VALUE_BOOL || value->type == VALUE_INT || value->type == VALUE_FLOAT ||
                value->type == VALUE_STRING) {
                returnValue = value_copy(value);
            } else {
                returnValue = value_ref(value);
            }
        }

        value_free(indexOrKey);
        value_free(containerValue);
        return returnValue;
    }
    if (node->type == NODE_CONST_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get(scope->block->env, node->lhs->string) != NULL) {
            error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't redeclare const variable: '%s'", node->lhs->string);
        }
        map_set(scope->block->env, node->lhs->string, variable_new(node->declarationType, false, value_ref(rhs)));
        return rhs;
    }
    if (node->type == NODE_LET_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (map_get(scope->block->env, node->lhs->string) != NULL) {
            error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't redeclare let variable: '%s'", node->lhs->string);
        }
        map_set(scope->block->env, node->lhs->string, variable_new(node->declarationType, true, value_ref(rhs)));
        return rhs;
    }
    if (node->type == NODE_ASSIGN) {
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);
        if (node->lhs->type == NODE_GET) {
            Value *containerValue = interpreter_node(interpreter, scope, node->lhs->lhs);
            if (containerValue->type != VALUE_ARRAY && containerValue->type != VALUE_OBJECT) {
                error(interpreter->text, node->token->line, node->token->position, "Variable is not an array or object it is: %s",
                      value_type_to_string(containerValue->type));
            }

            Value *indexOrKey = interpreter_node(interpreter, scope, node->lhs->rhs);
            if (containerValue->type == VALUE_ARRAY) {
                if (indexOrKey->type != VALUE_INT) {
                    error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Array index is not an int");
                }
                Value *previousValue = list_get(containerValue->array, indexOrKey->integer);
                if (previousValue != NULL) value_free(previousValue);
                list_set(containerValue->array, indexOrKey->integer, value_ref(rhs));
            }
            if (containerValue->type == VALUE_OBJECT) {
                if (indexOrKey->type != VALUE_STRING) {
                    error(interpreter->text, node->rhs->token->line, node->rhs->token->position, "Object key is not a string");
                }
                Value *previousValue = map_get(containerValue->object, indexOrKey->string);
                if (previousValue != NULL) value_free(previousValue);
                map_set(containerValue->object, indexOrKey->string, value_ref(rhs));
            }
            value_free(indexOrKey);
            value_free(containerValue);
        } else {
            Variable *variable = block_scope_get(scope->block, node->lhs->string);
            if (variable == NULL) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Variable: '%s' is not declared", node->lhs->string);
            }
            if (!variable->mutable) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Can't mutate const variable: '%s'", node->lhs->string);
            }
            if (variable->type != VALUE_ANY && variable->type != rhs->type) {
                error(interpreter->text, node->lhs->token->line, node->lhs->token->position, "Unexpected variable type: '%s' needed '%s'",
                      value_type_to_string(rhs->type), value_type_to_string(variable->type));
            }
            value_free(variable->value);
            variable->value = value_ref(rhs);
        }
        return rhs;
    }

    if (node->type >= NODE_ADD && node->type <= NODE_LOGICAL_OR) {
        Value *lhs = interpreter_node(interpreter, scope, node->lhs);
        Value *rhs = interpreter_node(interpreter, scope, node->rhs);

        if (node->type == NODE_ADD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer += rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating += rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating += rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer + rhs->floating;
                value_free(rhs);
                return lhs;
            }

            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                char *string = malloc(strlen(lhs->string) + strlen(rhs->string) + 1);
                stpcpy(string, lhs->string);
                strcat(string, rhs->string);
                value_clear(lhs);
                lhs->string = string;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_SUB) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer -= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating -= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating -= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer - rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_MUL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer *= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating *= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating *= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer * rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_EXP) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer = pow(lhs->integer, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->floating);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->floating, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = pow(lhs->integer, rhs->floating);
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_DIV) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer /= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating /= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating /= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = lhs->integer / rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_MOD) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer %= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->floating);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->floating, rhs->integer);
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_FLOAT;
                lhs->floating = fmod(lhs->integer, rhs->floating);
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_AND) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer &= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_XOR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer ^= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_OR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer |= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_SHL) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer <<= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_SHR) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->integer >>= rhs->integer;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_EQ) {
            if (lhs->type == VALUE_NULL) {
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = rhs->type == VALUE_NULL;
                value_free(rhs);
                return lhs;
            }
            if (rhs->type == VALUE_NULL) {
                bool result = lhs->type == VALUE_NULL;
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean == rhs->boolean;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer == rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating == rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                bool result = !strcmp(lhs->string, rhs->string);
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_NEQ) {
            if (lhs->type == VALUE_NULL) {
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = rhs->type != VALUE_NULL;
                value_free(rhs);
                return lhs;
            }
            if (rhs->type == VALUE_NULL) {
                bool result = lhs->type != VALUE_NULL;
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean != rhs->boolean;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer != rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating != rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_STRING && rhs->type == VALUE_STRING) {
                bool result = strcmp(lhs->string, rhs->string);
                value_clear(lhs);
                lhs->type = VALUE_BOOL;
                lhs->boolean = result;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_LT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating < rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer < rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_LTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating <= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer <= rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_GT) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating > rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer > rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_GTEQ) {
            if (lhs->type == VALUE_INT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->floating;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_FLOAT && rhs->type == VALUE_INT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->floating >= rhs->integer;
                value_free(rhs);
                return lhs;
            }
            if (lhs->type == VALUE_INT && rhs->type == VALUE_FLOAT) {
                lhs->type = VALUE_BOOL;
                lhs->boolean = lhs->integer >= rhs->floating;
                value_free(rhs);
                return lhs;
            }
        }

        if (node->type == NODE_LOGICAL_AND) {
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean && rhs->boolean;
                value_free(rhs);
                return lhs;
            }
        }
        if (node->type == NODE_LOGICAL_OR) {
            if (lhs->type == VALUE_BOOL && rhs->type == VALUE_BOOL) {
                lhs->boolean = lhs->boolean || rhs->boolean;
                value_free(rhs);
                return lhs;
            }
        }

        error(interpreter->text, node->token->line, node->token->position, "Type error");
    }

    fprintf(stderr, "Unkown node type: %d\n", node->type);
    exit(EXIT_FAILURE);
}