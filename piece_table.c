#include "piece_table.h"

// For debugging
void piece_dump(piece_table_t* pt, piece_t* current) {
    while (current) {
        const char* buffer = strcmp(current->data->source, "original_buffer") == 0
            ? pt->original_buffer
            : pt->append_buffer->content;

        printf("PIECE -> (");
        for (int i = 0; i < current->data->length; i++) {
            putchar(buffer[current->data->index + i]);
        }
        printf(")\n");

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

path_t* create_path(piece_table_t* pt) {
    path_t* path = malloc(sizeof(path_t));

    path_seq* path_seq = malloc(sizeof(path_seq));
    sequence_t* pt_copy = list_copy(pt->sequence->head);
    path_seq->head = pt_copy->head;

    path->head = path_seq;

    return path;
}

void add_path(path_t* path, piece_table_t* pt) {
    path_seq* path_seq = malloc(sizeof(path_seq));
    sequence_t* pt_copy = list_copy(pt->sequence->head);
    path_seq->head = pt_copy->head;

    path_seq->prev = path->head;
    path->head->next = path_seq;
    path->head = path_seq;
}

piece_t* create_piece(const char* source, int index, int length) {
    piece_t* piece = malloc(sizeof(piece_t));
    piece->data = malloc(sizeof(data_t));

    piece->data->source = strdup(source);
    piece->data->index = index;
    piece->data->length = length;

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
        0, strlen(original_buffer) 
    );

    pt->sequence->head = pt->sequence->tail = initial_piece;

    return pt;
};


void insert_text(piece_table_t* pt, char* data, int index, path_t* path) {
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
                current->data->length - first_half_len
            );

            piece_t* new_piece = create_piece(
                "append", 
                pt->append_buffer->length - strlen(data), 
                strlen(data) 
            );

            // splitting the piece: cutting the piece in the first half
            current->data->length = first_half_len;

            // last_half links
            if (current->next) {
                last_half->next = current->next;
                current->next->prev = last_half;
            }
            else pt->sequence->tail = last_half;
            last_half->prev = new_piece;

            // if last_half doesnt exist just not insert it
            if (last_half->data->length == 0) {
                last_half = current->next;
            }

            // if first_half doesnt exists does not insert it
            if (current->data->length == 0) {
                if (current->prev) {
                    current->prev->next = new_piece;
                    new_piece->prev = current->prev;
                } else pt->sequence->head = new_piece;
            } else {
                current->next = new_piece;
                new_piece->prev = current;
            }

            new_piece->next = last_half;

            pt->sequence->tail->next = NULL;

            add_path(path, pt);
            return;
        }

        // If index isn't in a piece and its in the end of the sequence
        if (current->next == NULL) {
            // creating the new piece
            piece_t* new_piece = create_piece(
                "append", 
                pt->append_buffer->length - strlen(data), 
                strlen(data) 
            );

            // inserting the new piece at the end
            new_piece->next = NULL;
            new_piece->prev = current;
            current->next = new_piece;
            pt->sequence->tail = new_piece;

            add_path(path, pt);
            return;
        }

        cur_pos += current->data->length;
        current = current->next;
    }
}

sequence_t* list_copy(piece_t* current) {
    sequence_t* sq = malloc(sizeof(sequence_t));
    piece_t* head = create_piece(                
        current->data->source, 
        current->data->index, 
        current->data->length 
    );
    sq->head = head;
    head->next = NULL;
    head->prev = current->prev ? current->prev : NULL;

    piece_t* tail = head;

    current = current->next;
    while (current) {
        piece_t* new_piece = create_piece(                
            current->data->source, 
            current->data->index, 
            current->data->length 
        );
        tail->next = new_piece;
        new_piece->prev = tail;

        tail = new_piece;
        sq->tail = tail;
        current = current->next;
    }

    return sq;
}

void delete_text(piece_table_t* pt, int index, int length, path_t* path) {
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
            // Saving the actual piece in the history
            piece_t* first_span = create_piece(
                current->data->source,
                current->data->index,
                index - piece_start
            );
            piece_t* last_span = create_piece(
                current->data->source,
                current->data->index + ((index + length) - piece_start),
                piece_end - (index + length)
            );

            // This is the piece where will be inserted the first_piece
            piece_t** insert_point = current->prev 
                ? &(current->prev->next) 
                : &(pt->sequence->head);

            // delete_info->mod_piece = *insert_point;

            if (current->next == NULL) 
                pt->sequence->tail = last_span;

            // If the piece is completely delected just jump to the next
            if (first_span->data->length == 0 && last_span->data->length == 0) {
                current->next->prev = (*insert_point)->prev;
                *insert_point = current->next;

                add_path(path, pt);
                return;
            } 

            if (first_span->data->length != 0 && last_span->data->length != 0) {
                *insert_point = first_span;
                first_span->next = last_span;
                if (current->prev)
                    first_span->prev = current->prev;
                last_span->next = current->next;
                if (current->next) {
                    current->next->prev = last_span;
                }
                last_span->prev = first_span;

                add_path(path, pt);
                return;
            }
            
            // There is not first_span and there is last_span
            if (first_span->data->length == 0) {
                *insert_point = last_span;
                if (current->prev)
                    last_span->prev = current->prev;
                last_span->next = current->next;

                add_path(path, pt);
                return;
            }

            // If there is first_span and not last_span
            if (last_span->data->length == 0) {
                *insert_point = first_span;
                if (current->prev)
                    first_span->prev = current->prev;
                first_span->next = current->next;
                add_path(path, pt);
                return;
            }
            return;
        }

        // When deleting is on various pieces (creating the pieces, then we'll join it).
        // Beginning of the deletion piece
        if (index >= piece_start && index <= piece_end) {
            first_piece = create_piece(
                current->data->source,
                current->data->index,
                index - piece_start
            );

            if (current->prev) {
                first_piece->prev = current->prev;
                first_piece->next = current;
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
                piece_end - (index + length)
            );

            last_piece->next = current->next;
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

    add_path(path, pt);
}

void undo(path_t* path, piece_table_t* pt) {
    if (path->head->prev == NULL) return;
    path->head = path->head->prev;
    pt->sequence->head = path->head->head;
}

void redo(path_t* path, piece_table_t* pt) {
    if (path->head->next == NULL) return;
    path->head = path->head->next;
    pt->sequence->head = path->head->head;
}

// FOR TESTING:
int main() {
    piece_table_t* pt = create_pt("");
    // history_t* h = create_history(pt);
    path_t* path = create_path(pt);

    insert_text(pt, "Hola buenos dias mi hermano querido", 0, path);
    insert_text(pt, "HOLA ESTO IRA POR EL MEDIO", 12, path);
    delete_text(pt, 12, 26, path);
    insert_text(pt, "ESTO DEBERIA IR AL PRINCIPIO ", 0, path);
    delete_text(pt, 0, 5, path);

    printf("THE ORIGINAL:\n");
    piece_dump(pt, pt->sequence->head);
}
