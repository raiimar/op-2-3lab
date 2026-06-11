#include "list.h"
#include "iterator.h"
#include "data.h"
#include <stdlib.h>
#include <string.h>

void insert_sorted(Node** sorted, Node* newNode, int columnIndex);
void update_list_tail(List* list, Node* sorted);

List* list_create(size_t dataSize) {
    List* list = (List*)malloc(sizeof(List));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->dataSize = dataSize;
        list->size = 0;
    }
    return list;
}

int list_push_back(List* list, void* data) {
    int result = 0;
    if (list != NULL && data != NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode) {
            newNode->data = malloc(list->dataSize);
            if (newNode->data)
                memcpy(newNode->data, data, list->dataSize);
            newNode->next = NULL;
            newNode->prev = list->tail;
            if (list->tail != NULL) {
                list->tail->next = newNode;
            } else {
                list->head = newNode;
            }
            list->tail = newNode;
            list->size++;
            result = 1;
        }
    }
    return result;
}

void list_clear(List* list) {
    if (list != NULL) {
        Node* current = list->head;
        while (current != NULL) {
            Node* toDelete = current;
            current = current->next;
            free(toDelete->data);
            free(toDelete);
        }
        free(list);
    }
}

void* list_first(List* list) {
    return (list && list->head) ? list->head->data : NULL;
}

void* list_last(List* list) {
    return (list && list->tail) ? list->tail->data : NULL;
}

double get_column_value(const void* row, int index) {
    const DataRow* r = (const DataRow*)row;
    double result = 0.0;
    if (r != NULL) {
        switch(index) {
            case COLUMN_YEAR:
                result = (double)r->year;
                break;
            case COLUMN_NATURAL_POPULATION_GROWTH:
                result = r->natural_population_growth;
                break;
            case COLUMN_BIRTH_RATE:
                result = r->birth_rate;
                break;
            case COLUMN_DEATH_RATE:
                result = r->death_rate;
                break;
            case COLUMN_GENERAL_DEMOGRAPHIC_WEIGHT:
                result = r->general_demographic_weight;
                break;
            case COLUMN_URBANIZATION:
                result = r->urbanization;
                break;
            default:
                result = 0.0;
                break;
        }
    }
    return result;
}


List* filter_to_list(List* list, int (*predicate)(const void*, const void*), const void* criterial) {
    List* result = NULL;
    if (list != NULL && predicate != NULL) {
        result = list_create(list->dataSize);
        if (result != NULL) {
            Iterator it = iterator_create(list);
            int error = 0;
            while (iterator_has_next(&it) && !error) {
                void* element = iterator_get(&it);
                if (predicate(element, criterial)) {
                    if (!list_push_back(result, element)) {
                        error = 1;
                    }
                }
                iterator_next(&it);
            }
            if (error) {
                list_clear(result);
                result = NULL;
            }
        }
    }
    return result;
}
void insert_sorted(Node** sorted, Node* newNode, int columnIndex) {
    double newValue = get_column_value(newNode->data, columnIndex);
    if (*sorted == NULL || get_column_value((*sorted)->data, columnIndex) >= newValue) {
        newNode->next = *sorted;
        if (*sorted != NULL) {
            (*sorted)->prev = newNode;
        }
        newNode->prev = NULL;
        *sorted = newNode;
    } else {
        Node* p = *sorted;
        while (p->next != NULL && get_column_value(p->next->data, columnIndex) < newValue) {
            p = p->next;
        }
        newNode->next = p->next;
        if (p->next != NULL) {
            p->next->prev = newNode;
        }
        p->next = newNode;
        newNode->prev = p;
    }
}

void update_list_tail(List* list, Node* sorted) {
    if (sorted == NULL) {
        list->tail = NULL;
    } else {
        Node* t = sorted;
        while (t->next != NULL) {
            t = t->next;
        }
        list->tail = t;
    }
}

void sort_list_by_column(List* list, int index) {
    if (list == NULL)
        return;
    Node* sorted = NULL;
    Node* current = list->head;
    while (current != NULL) {
        Node* next = current->next;
        insert_sorted(&sorted, current, index);
        current = next;
    }
    list->head = sorted;
    update_list_tail(list, sorted);
}

List* copy_list(List* source) {
    List* result = NULL;
    result = list_create(source->dataSize);
    if (result != NULL) {
        Iterator it = iterator_create(source);
        int error = 0;
        while (iterator_has_next(&it) && !error) {
            if(list_push_back(result, iterator_get(&it))) {
                iterator_next(&it);
            } else {
                error = 1;
            }
        }
        if (error) {
            list_clear(result);
            result = NULL;
        }
    }
    return result;
}
