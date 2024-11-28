#include "tui.h"
#include "string.h"

windows_size_t get_windows_size() {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    
    CONSOLE_SCREEN_BUFFER_INFO consoleInfo;
    
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
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {x, y};
    SetConsoleCursorPosition(hConsole, coord);
}

void render(char* content) {
    clear_console();

    vector_t* vec = create_vector(0);
    string_t* str = new_string(0);

    // Create the vector of the piece table information
    for (int i = 0; i < strlen(content); i++) {
        char c_to_str[2] = { content[i], '\0' };
        string_push(str, c_to_str);

        if (content[i] == '\n') {
            vector_push(vec, strdup(str->content));
            clear_string(str);
        }
    }
    // Print the vector
    for (int i = 0; i < vec->length; i++) {
        printf("%s", vec->entries[i]);
    }
}