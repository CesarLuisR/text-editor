#include <stdio.h>
#include "piece_table.h"
#include "tui.h"

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


void save_file(char* filename, char* content) {
    FILE* file = fopen(filename, "w");

    fprintf(file, "%s", content);
}

int main(int argc, char* argv[]) {
    if (file_handler(argc, argv) == 1) return 1;

    FILE* file = fopen(argv[1], "r");
    char* original_buffer = file_to_string(file);
    fclose(file);

    piece_table_t* pt = create_pt(original_buffer);
    // insert_text(pt, "// Esto deberia estar al principio del nuevo archivo\n", 0);
    char* pt_content = pt_parser(pt);

    save_file(argv[1], pt_content);

    render(pt_content);

    return 0;
}