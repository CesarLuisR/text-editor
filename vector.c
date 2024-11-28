#include "vector.h"

vector_t* create_vector(int cap) {
    vector_t* vec = malloc(sizeof(vector_t));
    vec->capacity = cap + 16;
    vec->length = 0;
    vec->entries = malloc(vec->capacity * sizeof(void*));

    return vec;
}

void vector_push(vector_t* vec, void* value) {
    if (vec->length >= vec->capacity) {
        vec->capacity *= 2;
        vec->entries = realloc(vec->entries, vec->capacity * sizeof(void*));
    }

    vec->entries[vec->length] = value;
    vec->length++;
}

void free_vector(vector_t* vec) {
    free(vec->entries);
    free(vec);
}

// int main() {
//     vector_t* vec = create_vector(0);
//     vector_push(vec, "Buenos dias");
//     printf(vec->entries[0]);
//     return 0;
// }