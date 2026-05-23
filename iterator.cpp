#include "iterator.h"
#include <stdlib.h>

Iterator iter_create(List* list) {
    Iterator it;
    it.current = NULL;
    if (list != NULL) {
        it.current = list->head;
    }
    return it;
}

int iter_has_next(Iterator* it) {
    int result = 0;
    if (it != NULL && it->current != NULL) {
        result = 1;
    }
    return result;
}

void iter_next(Iterator* it) {
    if (it != NULL && it->current != NULL) {
        it->current = it->current->next;
    }
}

void* iter_get(Iterator* it) {
    void* result = NULL;
    if (it != NULL && it->current != NULL) {
        result = it->current->data;
    }
    return result;
}
