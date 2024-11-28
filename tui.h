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
void render(char* content);