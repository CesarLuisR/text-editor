            // if (current->prev) {
            //     if (first_span->data->length == 0 && last_span->data->length == 0) {
            //         current->prev->next = current->next;
            //         if (current->prev->next == NULL)
            //             pt->sequence->tail = current->prev;
            //         return;
            //     }

            //     if (first_span->data->length == 0) {
            //         current->prev->next = last_span;
            //         last_span->prev = current->prev;
            //         last_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     } else {
            //         current->prev->next = first_span;
            //         first_span->next = last_span;
            //         first_span->prev = current->prev;
            //         last_span->next = current->next;
            //         last_span->prev = first_span;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     }

            //     if (last_span->data->length == 0) {
            //         current->prev->next = first_span;
            //         first_span->prev = current->prev;
            //         first_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = first_span;
            //         return;
            //     } else {
            //         current->prev->next = last_span;
            //         last_span->prev = current->prev;
            //         last_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     }
            // } else {
            //     if (first_span->data->length == 0 && last_span->data->length == 0) {
            //         pt->sequence->head = current->next;
            //         pt->sequence->head->prev = NULL;
            //         if (pt->sequence->head->next == NULL)
            //             pt->sequence->tail = pt->sequence->head;
            //         return;
            //     }

            //     if (first_span->data->length == 0) {
            //         pt->sequence->head = last_span;
            //         last_span->prev = NULL;
            //         last_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     } else {
            //         pt->sequence->head = first_span;
            //         first_span->next = last_span;
            //         first_span->prev = NULL;
            //         last_span->next = current->next;
            //         last_span->prev = first_span;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     }

            //     if (last_span->data->length == 0) {
            //         pt->sequence->head = first_span;
            //         first_span->prev = NULL;
            //         first_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = first_span;
            //         return;
            //     } else {
            //         pt->sequence->head = last_span;
            //         last_span->prev = NULL;
            //         last_span->next = current->next;
            //         if (current->next == NULL)
            //             pt->sequence->tail = last_span;
            //         return;
            //     }
            // }