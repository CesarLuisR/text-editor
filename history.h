#include "piece_table.h"

typedef struct {
    piece_table_t* pt;
    piece_t* head;
} history_t;

enum type_t {
    INSERT,
    DELETE
};

typedef struct {
    piece_t* next;
    piece_t* prev;
    void** value;
    enum type_t type;
    int imp;
} event_t;

history_t* create_history(piece_table_t* pt);
void history_add(history_t* history, void** value, enum type_t type);