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

char* pt_parser(piece_table_t* pt) {
    string_t* str = new_string(1000);

    piece_t* current = pt->sequence->head;
    while (current) {
        const char* buffer = strcmp(current->data->source, "original_buffer") == 0
            ? pt->original_buffer
            : pt->append_buffer->content;

        for (int i = 0; i < current->data->length; i++) {
            char c_to_str[2] = { buffer[current->data->index + i], '\0' };

            string_push(str, c_to_str);
        }

        current = current->next;
    }

    return str->content;
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
    strcat(original_buffer, "\0");

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

void insert_text(piece_table_t* pt, char* data, int index) {
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

            // if last_half doesnt exist just not insert it
            if (last_half->data->length == 0) {
                last_half = current->next;
            }

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

        // If index isn't in a piece and its in the end of the sequence
        if (current->next == NULL) {
            // creating the new piece
            int np_range = ++pt->sequence->range_counter;
            piece_t* new_piece = create_piece(
                "append", 
                pt->append_buffer->length - strlen(data), 
                strlen(data), 
                np_range
            );

            // inserting the new piece at the end
            new_piece->next = NULL;
            new_piece->prev = current;
            current->next = new_piece;
            pt->sequence->tail = new_piece;

            return;
        }

        cur_pos += current->data->length;
        current = current->next;
    }
}

void delete_text(piece_table_t* pt, int index, int length) {
    if (length == 0) return;

    piece_t* current = pt->sequence->head;
    int cur_pos = 0;

    piece_t* first_piece = NULL;
    piece_t* last_piece = NULL;

    while (current) {
        int piece_start = cur_pos;
        int piece_end = cur_pos + current->data->length;

        // if the deletion span is between a piece
        if (index >= piece_start && index + length <= piece_end) {
            piece_t* first_span = create_piece(
                current->data->source,
                current->data->index,
                index - piece_start,
                current->range
            );
            piece_t* last_span = create_piece(
                current->data->source,
                current->data->index + ((index + length) - piece_start),
                piece_end - (index + length),
                current->range
            );

            piece_t** insert_point = current->prev 
                ? &(current->prev->next) 
                : &(pt->sequence->head);

            if (*insert_point == NULL) 
                pt->sequence->tail = last_span;

            if (first_span->data->length == 0 && last_span->data->length == 0) {
                *insert_point = current->next;
                return;
            }
            
            if (first_span->data->length == 0) {
                *insert_point = last_span;
                if (current->prev)
                    last_span->prev = current->prev;
                last_span->next = current->next;
                return;
            } else {
                *insert_point = first_span;
                first_span->next = last_span;
                if (current->prev)
                    first_span->prev = current->prev;
                last_span->next = current->next;
                last_span->prev = first_span;
                return;
            }

            if (last_span->data->length == 0) {
                *insert_point = first_span;
                if (current->prev)
                    first_span->prev = current->prev;
                first_span->next = current->next;
                return;
            } else {
                *insert_point = last_span;
                if (current->prev)
                    last_span->prev = current->prev;
                last_span->next = current->next;
                return;
            }
        }

        // When deleting is on various pieces (creating the pieces, then we'll join it).
        // Beginning of the deletion piece
        if (index >= piece_start && index <= piece_end) {
            first_piece = create_piece(
                current->data->source,
                current->data->index,
                index - piece_start,
                current->range
            );

            if (current->prev) {
                first_piece->prev = current->prev;
                first_piece->next = current->next;
            } else {
                first_piece->prev = NULL;
                first_piece->next = current->next;
            }
        }

        // End of the deletion piece
        if (index + length >= piece_start && index + length <= piece_end) {
            last_piece = create_piece(
                current->data->source,
                current->data->index + ((index + length) - piece_start),
                piece_end - (index + length),
                current->range
            );

            last_piece->next = current->next;
        }

        // Pieces bewteen. For delete
        if (index + length - 1 > piece_start && index < piece_end) {
        }

        cur_pos += current->data->length;
        current = current->next;
    }

    // Joining the new pieces. This when the deleting span is across various pieces.
    if (first_piece == NULL) return;

    if (first_piece->prev) first_piece->prev->next = first_piece;
    else pt->sequence->head = first_piece;
    
    if (last_piece) {
        first_piece->next = last_piece;
        last_piece->prev = first_piece;
        
        if (last_piece->next == NULL) {
            pt->sequence->tail = last_piece;
        }
    }
}

// FOR TESTING:
int main() {
    piece_table_t* pt = create_pt("Hola a ti,");
    insert_text(pt, " tu y todos", 10);
    delete_text(pt, 0, 1);
    // piece_table_t* pt = create_pt("esto");
    // insert_piece(pt, "Hola ", 0);
    // insert_piece(pt, " deberia sentido", 9);
    // insert_piece(pt, " tener totalmente", 17);
    piece_dump(pt, pt->sequence->head);
    return 0;
}