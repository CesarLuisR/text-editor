#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "string.h"

typedef struct {
    char* source;
    int index;
    int length;
} data_t;

typedef struct piece_t {
    int range;
    data_t* data;
    struct piece_t* next;
    struct piece_t* prev;
} piece_t;

typedef struct {
    piece_t* head;
    piece_t* tail;
    int range_counter;
} sequence_t;

typedef struct {
    char* original_buffer;
    string_t* append_buffer;
    sequence_t* sequence;
} piece_table_t;

// Main functions
// piece_t* create_piece(const char* source, int index, int length, int range);
piece_table_t* create_pt(char* original_buffer);
void insert_piece(piece_table_t* pt, char* data, int index);

// Utilities
// void piece_dump(piece_table_t* pt, piece_t* current);
// void range_handler(piece_table_t* pt);

// El codigo le falta poder a;adir cosas al finan, muchos tests, y documentacion

