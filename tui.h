#include <windows.h>
#include "vector.h"

typedef struct {
    int width;
    int height;
} windows_size_t;

typedef struct {
    int x;
    int y;
} cursor_pos_t;

windows_size_t get_windows_size();
void gotoxy(int x, int y);
void clear_console();
void render(vector_t* vec, int* line_n, cursor_pos_t* c_pos, int* sp_line);
void listener(char* content, piece_table_t* pt, path_t* path);
void arrow_handler(int* line_n, cursor_pos_t* c_pos, int length, int* sp_line, vector_t* vec, int* cursor_max);