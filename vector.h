#include <stdio.h>
#include <stdlib.h>

typedef struct {
    int capacity;
    int length;
    void** entries;
} vector_t;

vector_t* create_vector(int cap);
void vector_push(vector_t* vec, void* value);
void free_vector(vector_t* vec);