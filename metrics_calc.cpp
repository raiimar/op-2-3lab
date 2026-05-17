#include "metrics_calc.h"
#include "context_state.h"

#include <stdlib.h>
#include <string.h>

void calc_metrics(List* list, int columnIndex, double* min, double* max, double* median) {
    *min = get_column_value(&list->head->data, columnIndex);
    *max = get_column_value(&list->tail->data, columnIndex);
    int mid = (list->size - 1) / 2;
    Node* midNode = list->head;
    for (int i = 0; i < mid; ++i) {
        midNode = midNode->next;
    }
    double v1 = get_column_value(&midNode->data, columnIndex);
    if (list->size % 2 == 1) {
        *median = v1;
    } else {
        double v2 = get_column_value(&midNode->next->data, columnIndex);
        *median = (v1 + v2) / 2;
    }
}

void logic_calculate_metrics(AppContext* context, const char* region, int columnIndex, int startYear, int endYear) {
    int success = 1;
    List* filteredList = NULL;
    YearRange years = {startYear, endYear};
    FilteredSeries series = {NULL, NULL, 0};

    if (context->dataList == NULL) {
        set_status_message(context, ERROR_INVALID_DATA, "No data loaded.");
        success = 0;
    } else if (startYear >= endYear) {
        set_status_message(context, ERROR_INVALID_PARAMS, "Invalid year range.");
        success = 0;
    }

    if (success) {
        reset_metrics(context);
        clear_plot_buffers(context);

        filteredList = filter_to_list(context->dataList, region, years);
        if (filteredList == NULL) {
            set_status_message(context, ERROR_EMPTY_RESULT, "No data for given region");
            success = 0;
        }
    }

    if (success) {
        sort_list_by_column(filteredList, columnIndex);
        calc_metrics(filteredList, columnIndex,
                     &context->metrics.min,
                     &context->metrics.max,
                     &context->metrics.median);

        if (!list_to_series(filteredList, columnIndex, &series)) {
            set_status_message(context, ERROR_INVALID_DATA, "Failed to extract series for plot.");
            success = 0;
        } else {
            context->plot.years = series.years;
            context->plot.values = series.values;
            context->plot.count = series.count;
            context->plot.columnIndex = columnIndex;
            strncpy(context->plot.region, region, REGION_NAME_LENGTH - 1);
            context->plot.region[REGION_NAME_LENGTH - 1] = '\0';
            if (series.count > 0) {
                context->plot.yearMin = series.years[0];
                context->plot.yearMax = series.years[series.count - 1];
            } else {
                context->plot.yearMin = 0;
                context->plot.yearMax = 0;
            }
        }
    }

    if (filteredList != NULL) {
        list_clear(filteredList);
    }

    if (success) {
        set_status_message(context, STATUS_OK, "Metrics calculated.");
    }
}
