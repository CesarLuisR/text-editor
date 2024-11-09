#include <stdio.h>
#include "string.h"

int file_handler(int argc, char* argv[]) {
    if (argc == 1) {
        printf("Enter an argument");
        return 1;
    }

    return 0;
}

char* file_to_string(FILE* file) {
    string_t* original_buffer = new_string(256);
    char line[256];

    while (fgets(line, sizeof(line), file)) {
        string_push(original_buffer, line);
    }

    return original_buffer->content;
}

int main(int argc, char* argv[]) {
    if (file_handler(argc, argv) == 1) return 1;

    FILE* file = fopen(argv[1], "r");
    char* original_buffer = file_to_string(file);

    printf("%s", original_buffer);

    return 0;
}