#ifndef LIST_H
#define LIST_H

#include <stddef.h>

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
    size_t dataSize;
} List;

List* list_create(size_t dataSize); //
int list_push_back(List* list, void* data);
void list_clear(List* list);
void* list_first(List* list);
void* list_last(List* list);

double get_column_value(const void* row, int columnIndex);
int matches_filter(const void* row, const char* region, int startYear, int endYear); //
int add_filtered_item(List* list, void* row);
List* filter_to_list(List* list, const char* region, int startYear, int endYear); //
void sort_list_by_column(List* list, int columnIndex); //
List* copy_list(List* source);

#endif
