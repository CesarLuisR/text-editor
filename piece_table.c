#include "piece_table.h"

void piece_dump(piece_table_t* pt, piece_t* current) {
    while (current) {
        const char* buffer = strcmp(current->data->source, "original_buffer") == 0
            ? pt->original_buffer
            : pt->append_buffer->content;

        printf("PIECE -> (");
        for (int i = 0; i < current->data->length; i++) {
            putchar(buffer[current->data->index + i]);
        }
        printf(") -> range: %d\n", current->range);

        current = current->next;
    }
}

piece_t* create_piece(const char* source, int index, int length, int range) {
    piece_t* piece = malloc(sizeof(piece_t));
    piece->data = malloc(sizeof(data_t));

    piece->data->source = strdup(source);
    piece->data->index = index;
    piece->data->length = length;
    piece->range = range;

    piece->next = NULL;
    piece->prev = NULL;

    return piece;
}

piece_table_t* create_pt(char* original_buffer) {
    piece_table_t* pt = malloc(sizeof(piece_table_t));

    pt->original_buffer = strdup(original_buffer);
    pt->append_buffer = new_string(0);
    pt->sequence = malloc(sizeof(sequence_t));

    pt->sequence->range_counter = 0;
    piece_t* initial_piece = create_piece(
        "original_buffer", 
        0, strlen(original_buffer), 
        pt->sequence->range_counter
    );

    pt->sequence->head = pt->sequence->tail = initial_piece;

    return pt;
};

void insert_piece(piece_table_t* pt, char* data, int index) {
    string_push(pt->append_buffer, data);

    piece_t* current = pt->sequence->head;
    int cur_pos = 0;

    while (current) {
        // Detecting the piece for insertion
        if (cur_pos + current->data->length > index) {
            int first_half_len = index - cur_pos;
            piece_t* last_half = create_piece(
                current->data->source,
                current->data->index + first_half_len,
                current->data->length - first_half_len,
                current->range
            );

            int np_range = ++pt->sequence->range_counter;
            piece_t* new_piece = create_piece(
                "append", 
                pt->append_buffer->length - strlen(data), 
                strlen(data), 
                np_range
            );

            // splitting the piece: cutting the piece in the first half
            current->data->length = first_half_len;

            // last_half links
            if (current->next) last_half->next = current->next;
            else pt->sequence->tail = last_half;
            last_half->prev = new_piece;

            // if first_half doesnt exists does not insert it
            if (current->data->length == 0) {
                if (current->prev) current->prev->next = new_piece;
                else pt->sequence->head = new_piece;
            } else current->next = new_piece;
            
            new_piece->next = last_half;
            new_piece->prev = current;

            pt->sequence->tail->next = NULL;

            return;
        }

        // If index isn't in a piece
        if (current == pt->sequence->tail && cur_pos + current->data->length - 1 < index) {
            int np_range = ++pt->sequence->range_counter;
            piece_t* new_piece = create_piece(
                "append", 
                pt->append_buffer->length - strlen(data), 
                strlen(data), 
                np_range
            );

            new_piece->prev = current;
            current->next = new_piece;
            new_piece->next = NULL;
            pt->sequence->tail = new_piece;

            return;
        }

        cur_pos += current->data->length;
        current = current->next;
    }
}

int main() {
    piece_table_t* pt = create_pt("A");
    insert_piece(pt, "B", 1);
    insert_piece(pt, "C", 2);
    insert_piece(pt, "D", 3);
    piece_dump(pt, pt->sequence->head);
    return 0;
}