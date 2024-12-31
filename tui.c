#include "tui.h"
#include "string.h"
// #include "piece_table.h"
#include <conio.h>

// THIS FILE HAS SHIT CODE I KNOW, SORRY I AM THROUGH A BREAKUP :( 12/30/2024
// THE UNDO/REDO IS SHIT BECAUSE OF THIS, I DONT WANT TO KEEP DOING THIS ANYMORE
// BUT I NEED TO FINISH THIS BEFORE 2025 :) 

windows_size_t get_windows_size() {
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    GetConsoleScreenBufferInfo(hConsole, &consoleInfo);

    int width = consoleInfo.srWindow.Right - consoleInfo.srWindow.Left + 1;
    int height = consoleInfo.srWindow.Bottom - consoleInfo.srWindow.Top + 1;

    windows_size_t w_size = { width, height };

    return w_size;
}

void clear_console() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD written;
    DWORD consoleSize;

    // Obtener información del buffer de la consola
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    // Calcular el tamaño total de la consola
    consoleSize = csbi.dwSize.X * csbi.dwSize.Y;
    // Rellenar con espacios
    FillConsoleOutputCharacter(hConsole, ' ', consoleSize, (COORD){0, 0}, &written);
    // Restaurar el cursor a la posición inicial
    SetConsoleCursorPosition(hConsole, (COORD){0, 0});
}

void gotoxy(int x, int y) {
    COORD coord;
    coord.X = x; // Columna
    coord.Y = y; // Fila
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("\033[%d;%dH", y, x);
}

void render(vector_t* vec, int* line_n, cursor_pos_t* c_pos, int* sp_line) {
    windows_size_t w_size = get_windows_size();
    // Print the vector
    clear_console();

    int condition = vec->length > *line_n + (w_size.height - 1)
        ? *line_n + (w_size.height - 1)
        : vec->length;
    
    for (int i = *line_n; i < condition; i++) {
        if (vec->entries[i] == NULL) break;
        printf("%*d %c %s", 5, i + 1, 179, vec->entries[i]);
    }

    // gotoxy(c_pos->x, c_pos->y);
    gotoxy(c_pos->x, c_pos->y);
}

void arrow_handler(int* line_n, cursor_pos_t* c_pos, int length, int* sp_line, vector_t* vec, int* cursor_max) {
    windows_size_t w_size = get_windows_size();
    const int init_x = 8;

    int c = _getch();
    switch (c) {
        case 72: {
            // Up
            if (c_pos->y == 4) {
                if (*line_n == 0) {
                    if (c_pos->y == 1) return;
                    if (*cursor_max > strlen(vec->entries[*sp_line - 2])) {
                        c_pos->x = strlen(vec->entries[*sp_line - 2]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                    c_pos->y = c_pos->y - 1;
                    *sp_line = *sp_line - 1;
                    return;
                }
                    if (*cursor_max > strlen(vec->entries[*sp_line - 2])) {
                        c_pos->x = strlen(vec->entries[*sp_line - 2]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                *line_n = *line_n - 1;
                *sp_line = *sp_line - 1;
            } else {
                if (c_pos->y == 1) return;
                    if (*cursor_max > strlen(vec->entries[*sp_line - 2])) {
                        c_pos->x = strlen(vec->entries[*sp_line - 2]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                c_pos->y = c_pos->y - 1;
                *sp_line = *sp_line - 1;
            }
            break;
        }
        case 80: // Down
            if (c_pos->y == w_size.height - 4) {
                if (length - *line_n < w_size.height) {
                    if (c_pos->y == w_size.height) return;
                    if (*cursor_max > strlen(vec->entries[*sp_line])) {
                        c_pos->x = strlen(vec->entries[*sp_line]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                    c_pos->y = c_pos->y + 1;
                    *sp_line = *sp_line + 1;
                    return;
                };
                    if (*cursor_max > strlen(vec->entries[*sp_line])) {
                        c_pos->x = strlen(vec->entries[*sp_line]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                *line_n = *line_n + 1;
                *sp_line = *sp_line + 1;
            } else {
                if (c_pos->y == w_size.height) return;
                    if (*cursor_max > strlen(vec->entries[*sp_line])) {
                        c_pos->x = strlen(vec->entries[*sp_line]) + 8;
                    } else {
                        c_pos->x = *cursor_max + 8;
                    }
                c_pos->y = c_pos->y + 1;
                *sp_line = *sp_line + 1;
            }
            break;
        case 75: // Left
            if (c_pos->x == 9) return;
            *cursor_max = *cursor_max - 1;
            c_pos->x = c_pos->x - 1;
            break;
        case 77: // right
            if (strlen(vec->entries[*sp_line - 1]) == c_pos->x - 8) return;
            *cursor_max = *cursor_max + 1;
            c_pos->x = c_pos->x + 1;
            break;
    }
}

void listener(char* content, piece_table_t* pt, path_t* path) {
    int line_n = 0;
    int sp_line = 1;
    cursor_pos_t c_pos = {9, 1};
    int cursor_max = 1;

    while (1) {
        vector_t* vec = create_vector(0);
        string_t* str = new_string(0);

        // Create the vector of the piece table information
        for (int i = 0; i < strlen(content); i++) {
            char c_to_str[2] = { content[i], '\0' };
            string_push(str, c_to_str);

            if (i + 1 == strlen(content)) {
                vector_push(vec, strdup(str->content));
                break;
            }

            if (content[i] == '\n') {
                vector_push(vec, strdup(str->content));
                clear_string(str);
            }
        }

        int pt_id = 0;
        for (int i = 0; i < sp_line - 1; i++) {
            pt_id += strlen(vec->entries[i]);
        }
        pt_id += c_pos.x - 9;
        render(vec, &line_n, &c_pos, &sp_line);


        windows_size_t w_size = get_windows_size();
        printf("\033[%d;%dH", w_size.height, 0);
        printf("%d %d", pt_id, strlen(vec->entries[1]));
        printf("\033[%d;%dH", c_pos.y, c_pos.x);

        int c = getch(); 
        switch (c) {
            case 27: // Escape
                exit(0);
                break;
            case 224: // for the arrow handler
                arrow_handler(&line_n, &c_pos, vec->length, &sp_line, vec, &cursor_max);
                break;
            case 13: // Enter
                break;
            case 8: // Backspace
                break;
            case 26:
                break;

            default: // Any letter
                break;
        }
    }
}