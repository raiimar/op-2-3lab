#ifndef ITERATOR_H
#define ITERATOR_H

#include "list.h"

typedef struct {
    Node* current;
} Iterator;

Iterator create(List* list);
int has_next(Iterator* it);
void next(Iterator* it);
DataRow* get(Iterator* it);

#endif
