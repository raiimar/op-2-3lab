#ifndef ITERATOR_H
#define ITERATOR_H

#include "list.h"

typedef struct {
    Node* current;
} Iterator;

Iterator iterator_create(List* list);
int iterator_has_next(Iterator* it);
void iterator_next(Iterator* it);
void* iterator_get(Iterator* it);

#endif
