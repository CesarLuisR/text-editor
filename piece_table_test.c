#include "piece_table.h"

int main() {
    piece_table_t* pt = create_pt("Hola buenos dias");
    printf("range: %d, original buffer %s", pt->sequence->head->range, pt->original_buffer);
}