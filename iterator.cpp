#include "iterator.h"
#include <stdlib.h>

Iterator create(List* list) {
    Iterator it;
    it.current = NULL;
    if (list != NULL) {
        it.current = list->head;
    }
    return it;
}

int has_next(Iterator* it) {
    return (it != NULL && it->current != NULL);
}

void next(Iterator* it) {
    if (it != NULL && it->current != NULL) {
        it->current = it->current->next;
    }
}

DataRow* get(Iterator* it) {
    DataRow* result = NULL;
    if (it != NULL && it->current != NULL) {
        result = &(it->current->data);
    }
    return result;
}
