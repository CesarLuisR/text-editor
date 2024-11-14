#include <stdlib.h>
#include <string.h>
#include <stdio.h>

typedef struct {
    int capacity;
    int length;
    char* content;
} string_t;

string_t* new_string(int init_cap);
void string_push(string_t* string, char* new_cont);
void clear_string(string_t* str);