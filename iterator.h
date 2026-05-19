#ifndef ITERATOR_H
#define ITERATOR_H

#include "list.h"

typedef struct {
    Node* current;
} Iterator;

Iterator iter_create(List* list); //
int iter_has_next(Iterator* it);
void iter_next(Iterator* it);
DataRow* iter_get(Iterator* it);

#endif
