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

enum type_t {
    INSERT,
    DELETE
};

typedef struct event_t {
    struct event_t* next;
    struct event_t* prev;
    void* value;
    enum type_t type;
    int imp;
} event_t;

typedef struct {
    piece_table_t* pt;
    event_t* head;
} history_t;

typedef struct {
    int index;
    char* data;
} insert_info_t;

// typedef struct {
//     sequence_t* actual_piece;
//     sequence_t* mod_piece;
//     piece_t* insert_p;
// } delete_info_t;

typedef struct path_seq {
    piece_t* head;
    piece_t* tail;
    struct path_seq* next;
    struct path_seq* prev;
} path_seq;

typedef struct {
    path_seq* head;
    path_seq* tail;
} path_t;

typedef struct {
    int index;
    int length;
    char* data;
} delete_info_t;

// Main functions
piece_t* create_piece(const char* source, int index, int length);
piece_table_t* create_pt(char* original_buffer);
void insert_text(piece_table_t* pt, char* data, int index, path_t* path);
void delete_text(piece_table_t* pt, int index, int length, path_t* path);
void undo(path_t* path, piece_table_t* pt);
void redo(path_t* path, piece_table_t* pt);

// Utilities
void piece_dump(piece_table_t* pt, piece_t* current);
char* pt_parser(piece_table_t* pt);
sequence_t* list_copy(piece_t* current);



