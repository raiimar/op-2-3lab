#include "list.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>

List* list_create() {
    List* list = (List*)malloc(sizeof(List));
    if (list != NULL) {
        list->head = NULL;
        list->tail = NULL;
        list->size = 0;
    }
    return list;
}

int list_push_back(List* list, void* data) {
    int result = 0;
    if (list != NULL && data != NULL) {
        Node* newNode = (Node*)malloc(sizeof(Node));
        if (newNode != NULL) {
            newNode->data = data;
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


int matches_filter(const DataRow* row, const char* region, int startYear, int endYear) {
    int result = 0;
    if (row != NULL && row->year >= startYear && row->year <= endYear) {
        if (strcmp(row->region, region) == 0) {
            result = 1;
        }
    }
    return result;
}

int add_filtered_item(List* list, DataRow* row) {
    int result = 0;
    DataRow* copy = (DataRow*)malloc(sizeof(DataRow));
    if (copy != NULL) {
        *copy = *row;
        if (list_push_back(list, copy)) {
            result = 1;
        } else {
            free(copy);
        }
    }
    return result;
}

double get_column_value(const DataRow* row, int columnIndex) {
    double result = 0.0;
    if (row != NULL) {
        if (columnIndex == COLUMN_YEAR) {
            result = (double)row->year;
        } else if (columnIndex == COLUMN_NATURAL_POPULATION_GROWTH) {
            result = row->natural_population_growth;
        } else if (columnIndex == COLUMN_BIRTH_RATE) {
            result = row->birth_rate;
        } else if (columnIndex == COLUMN_DEATH_RATE) {
            result = row->death_rate;
        } else if (columnIndex == COLUMN_GENERAL_DEMOGRAPHIC_WEIGHT) {
            result = row->general_demographic_weight;
        } else if (columnIndex == COLUMN_URBANIZATION) {
            result = row->urbanization;
        }
    }
    return result;
}

int collect_filtered(List* result, List* list, const char* region, int startYear, int endYear) {
    int success = 1;
    Iterator it = iterator_create(list);
    while (iterator_has_next(&it) && success) {
        DataRow* row = (DataRow*)iterator_get(&it);
        if (matches_filter(row, region, startYear, endYear)) {
            if (!add_filtered_item(result, row)) {
                success = 0;
            }
        }
        iterator_next(&it);
    }
    return success;
}

List* filter_to_list(List* list, const char* region, int startYear, int endYear) {
    List* result = NULL;
    if (list != NULL && region != NULL) {
        result = list_create();
        if (result != NULL) {
            if (!collect_filtered(result, list, region, startYear, endYear)) {
                list_clear(result);
                result = NULL;
            }
        }
    }
    return result;
}

void insert_sorted(Node** sorted, Node* newNode, int columnIndex) {
    double newValue = get_column_value((DataRow*)newNode->data, columnIndex);
    if (*sorted == NULL || get_column_value((DataRow*)(*sorted)->data, columnIndex) >= newValue) {
        newNode->next = *sorted;
        if (*sorted != NULL) {
            (*sorted)->prev = newNode;
        }
        newNode->prev = NULL;
        *sorted = newNode;
    } else {
        Node* p = *sorted;
        while (p->next != NULL && get_column_value((DataRow*)p->next->data, columnIndex) < newValue) {
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

void sort_list_by_column(List* list, int columnIndex) {
    Node* sorted = NULL;
    Node* current = list->head;

    while (current != NULL) {
        Node* next = current->next;
        insert_sorted(&sorted, current, columnIndex);
        current = next;
    }

    list->head = sorted;
    update_list_tail(list, sorted);
}
