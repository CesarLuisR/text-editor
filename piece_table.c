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

history_t* create_history(piece_table_t* pt) {
    history_t* history = malloc(sizeof(history));
    history->head = NULL;
    return history;
}

void history_add(history_t* history, void* value, enum type_t type) {
    event_t* ev = malloc(sizeof(event_t));
    ev->imp = 1;
    ev->type = type;
    ev->value = value;
    ev->next = ev->prev = NULL;

    if (history->head) {
        ev->next = history->head;
        history->head->prev = ev;
    }

    history->head = ev;
}

void dimp(history_t* h, event_t* ev, piece_table_t* pt) {
    if (ev->imp == 0) return;

    ev->imp = 0;
    if (ev->type == INSERT) {
        insert_info_t* current = ((insert_info_t*)(ev->value));
        delete_text(pt, current->index, strlen(current->data), h);
    } else {
        delete_info_t* current = ((delete_info_t*)(ev->value));

        if (current->actual_piece->prev) {
            current->actual_piece->prev->next = current->actual_piece;
        } else {
            pt->sequence->head = current->actual_piece;
        }
    }
}

void imp(history_t* h, event_t* ev, piece_table_t* pt) {
    if (ev->imp == 1) return;

    ev->imp = 1;
    if (ev->type == INSERT) {
        insert_info_t* current = ((insert_info_t*)(ev->value));
        insert_text(pt, current->data, current->index, h);
    } else {
        delete_info_t* current = ((delete_info_t*)(ev->value));

        if (current->actual_piece->prev) {
            current->actual_piece->prev->next = current->mod_piece;
        } else {
            pt->sequence->head = current->mod_piece;
        }
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


void insert_text(piece_table_t* pt, char* data, int index, history_t* h) {
    string_push(pt->append_buffer, data);

    insert_info_t* insert_info = malloc(sizeof (insert_info_t));
    insert_info->data = data;
    insert_info->index = index;

    history_add(h, insert_info, INSERT);

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

piece_t* list_copy(piece_t* current) {
    piece_t* head = create_piece(                
        current->data->source, 
        current->data->index, 
        current->data->length, 
        current->range
    );
    head->next = NULL;
    
    piece_t* tail = head;

    current = current->next;
    while (current) {
        piece_t* new_piece = create_piece(                
            current->data->source, 
            current->data->index, 
            current->data->length, 
            current->range
        );
        tail->next = new_piece;
        new_piece->prev = tail;

        tail = new_piece;
        current = current->next;
    }

    return head;
}

void delete_text(piece_table_t* pt, int index, int length, history_t* h) {
    if (length == 0) return;

    piece_t* current = pt->sequence->head;
    int cur_pos = 0;

    piece_t* first_piece = NULL;
    piece_t* last_piece = NULL;

    delete_info_t* delete_info = malloc(sizeof(delete_info_t));

    while (current) {
        int piece_start = cur_pos;
        int piece_end = cur_pos + current->data->length;

        // if the deletion span is between a piece
        if (index >= piece_start && index + length <= piece_end) {
            // Saving the actual piece in the history
            delete_info->actual_piece = list_copy(current);
            delete_info->actual_piece->prev = current->prev;

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

            // This is the piece where will be inserted the first_piece
            piece_t** insert_point = current->prev 
                ? &(current->prev->next) 
                : &(pt->sequence->head);

            // delete_info->mod_piece = *insert_point;

            if (current->next == NULL) 
                pt->sequence->tail = last_span;

            // If the piece is completely delected just jump to the next
            if (first_span->data->length == 0 && last_span->data->length == 0) {
                *insert_point = current->next;
                current->next->prev = *insert_point;

                delete_info->mod_piece = list_copy(*insert_point);
                history_add(h, delete_info, DELETE);
                return;
            } else {
                *insert_point = first_span;
                first_span->next = last_span;
                if (current->prev)
                    first_span->prev = current->prev;
                last_span->next = current->next;
                last_span->prev = first_span;

                delete_info->mod_piece = list_copy(*insert_point);
                history_add(h, delete_info, DELETE);
                return;
            }
            
            // There is not first_span and there is last_span
            if (first_span->data->length == 0) {
                *insert_point = last_span;
                if (current->prev)
                    last_span->prev = current->prev;
                last_span->next = current->next;

                delete_info->mod_piece = list_copy(*insert_point);
                history_add(h, delete_info, DELETE);
                return;
            }

            // If there is first_span and not last_span
            if (last_span->data->length == 0) {
                *insert_point = first_span;
                if (current->prev)
                    first_span->prev = current->prev;
                first_span->next = current->next;

                delete_info->mod_piece = list_copy(*insert_point);
                delete_info->mod_piece->prev = *insert_point;
                history_add(h, delete_info, DELETE);
                return;
            }
        }

        // When deleting is on various pieces (creating the pieces, then we'll join it).
        // Beginning of the deletion piece
        if (index >= piece_start && index <= piece_end) {
            delete_info->actual_piece = list_copy(current);
            delete_info->actual_piece->prev = current->prev;

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

        // Pieces bewteen. For delete. Needed????
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

    delete_info->mod_piece = list_copy(first_piece);
    history_add(h, delete_info, DELETE);
}

// FOR TESTING:
int main() {
    piece_table_t* pt = create_pt("");
    history_t* h = create_history(pt);

    insert_text(pt, "Hola buenos dias mi hermano querido", 0, h);
    insert_text(pt, " ENTONCES AQUI VA LO SIGUIENTE", 34, h);
    delete_text(pt, 34, 30, h);

    event_t* current = h->head;
    dimp(h, current, pt);
    imp(h, current, pt);
    dimp(h, current, pt);
    imp(h, current, pt);

    piece_dump(pt, pt->sequence->head);
    return 0;
}