
#include "string.h"

string_t* new_string(int init_cap) {
    string_t* string = malloc(sizeof(string_t));
    string->capacity = init_cap;
    string->content = malloc(sizeof(char) * init_cap);
    string->content[0] = '\0';
    string->length = 0;
    return string;
}

void string_push(string_t* string, char* new_cont) {
    if (string->length + strlen(new_cont) > string->capacity) {
        string->capacity += strlen(new_cont);
        string->capacity *= 2;
        string->content = realloc(string->content, string->capacity);
    }

    strcat(string->content, new_cont);
    strcat(string->content, "\0");

    string->length = strlen(string->content);
}