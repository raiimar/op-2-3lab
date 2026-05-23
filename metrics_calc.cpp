#include "metrics_calc.h"
#include "context_state.h"
#include "iterator.h"
#include <stdlib.h>
#include <string.h>

static List* copy_list(List* source) {
    List* result = NULL;
    Iterator it;
    if (source != NULL) {
        result = list_create();
        if (result != NULL) {
            it = iterator_create(source);
            while (iterator_has_next(&it)) {
                DataRow* row = (DataRow*)iterator_get(&it);
                DataRow* copy = (DataRow*)malloc(sizeof(DataRow));
                if (copy != NULL) {
                    *copy = *row;
                    if (!list_push_back(result, copy)) {
                        free(copy);
                        list_clear(result);
                        result = NULL;
                        break;
                    }
                } else {
                    list_clear(result);
                    result = NULL;
                    break;
                }
                iterator_next(&it);
            }
        }
    }
    return result;
}

void calc_metrics(List* list, int columnIndex, double* min, double* max, double* median) {
    Node* current = list->head;
    *min = get_column_value((DataRow*)current->data, columnIndex);
    *max = *min;
    int count = 0;
    while (current != NULL) {
        double val = get_column_value((DataRow*)current->data, columnIndex);
        if (val < *min) *min = val;
        if (val > *max) *max = val;
        count++;
        current = current->next;
    }

    List* sorted = copy_list(list);
    if (sorted != NULL) {
        sort_list_by_column(sorted, columnIndex);
        int mid = (sorted->size - 1) / 2;
        Node* midNode = sorted->head;
        for (int i = 0; i < mid; ++i) {
            midNode = midNode->next;
        }
        double v1 = get_column_value((DataRow*)midNode->data, columnIndex);
        if (sorted->size % 2 == 1) {
            *median = v1;
        } else {
            double v2 = get_column_value((DataRow*)midNode->next->data, columnIndex);
            *median = (v1 + v2) / 2.0;
        }
        list_clear(sorted);
    } else {
        *median = 0.0;
    }
}

void logic_calculate_metrics(AppContext* context, const AppParams* params) {
    int success = 1;
    List* filteredList = NULL;

    if (params->startYear >= params->endYear) {
        set_status_message(context, ERROR_INVALID_PARAMS);
        success = 0;
    }

    if (success) {
        reset_metrics(context);
        clear_plot_buffers(context);
        context->plot.columnIndex = (DataColumnNumbers)params->columnIndex;

        filteredList = filter_to_list(context->dataList, params->region,
                                      params->startYear, params->endYear);
        if (filteredList == NULL || filteredList->size == 0) {
            set_status_message(context, ERROR_EMPTY_RESULT);
            if (filteredList != NULL) list_clear(filteredList);
            success = 0;
        } else {
            sort_list_by_column(filteredList, COLUMN_YEAR);
            context->plot.filteredData = filteredList;
            strncpy(context->plot.region, params->region, REGION_NAME_LENGTH - 1);
            context->plot.region[REGION_NAME_LENGTH - 1] = '\0';
            context->plot.yearMin = ((DataRow*)filteredList->head->data)->year;
            context->plot.yearMax = ((DataRow*)filteredList->tail->data)->year;

            calc_metrics(filteredList, params->columnIndex,
                         &context->metrics.min, &context->metrics.max, &context->metrics.median);
        }
    }

    if (!success && filteredList != NULL) {
        list_clear(filteredList);
    }

    if (success) {
        set_status_message(context, STATUS_OK);
    }
}
