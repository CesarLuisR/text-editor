#include "string.h"

string_t* new_string(int init_cap) {
    string_t* string = malloc(sizeof(string_t));
    string->capacity = init_cap + 16;
    string->content = malloc(sizeof(char) * string->capacity);
    string->content[0] = '\0';
    string->length = 0;
    return string;
}

void string_push(string_t* string, char* new_cont) {
    if (string->length + strlen(new_cont) >= string->capacity) {
        string->capacity += strlen(new_cont);
        string->capacity *= 2;
        string->content = realloc(string->content, string->capacity);
    }

    strcat(string->content, new_cont);

    string->length = strlen(string->content);
}

void clear_string(string_t* str) {
    str->length = 0;
    strcpy(str->content, "");
}

// int main(void) {
//     string_t* n = new_string(0);
//     string_push(n, "Hola buenos dias mi hermano");
//     printf(n->content);
//     clear_string(n);
//     printf("\nString vacia: %s\n", n->content);
//     string_push(n, "Y entonces");
//     string_push(n, "Otro mas klk");
//     printf(n->content);
//     clear_string(n);
//     printf("\nString vacia: %s\n", n->content);
//     string_push(n, "Hola buenos dias mi hermano");
//     printf(n->content);
//     return 0;
// }