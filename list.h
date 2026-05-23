#ifndef LIST_H
#define LIST_H

#include "data.h"

typedef struct Node {
    void* data;
    struct Node* next;
    struct Node* prev;
} Node;

typedef struct {
    Node* head;
    Node* tail;
    int size;
} List;

List* list_create();
int list_push_back(List* list, void* data);
void list_clear(List* list);

double get_column_value(const DataRow* row, int columnIndex);
int collect_filtered(List* result, List* list, const char* region, int startYear, int endYear);
List* filter_to_list(List* list, const char* region, int startYear, int endYear);
void sort_list_by_column(List* list, int columnIndex);
int matches_filter(const DataRow* row, const char* region, int startYear, int endYear);
int add_filtered_item(List* list, DataRow* row);
void insert_sorted(Node** sorted, Node* newNode, int columnIndex);
void update_list_tail(List* list, Node* sorted);

#endif
