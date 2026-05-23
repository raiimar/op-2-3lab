#include "metrics_calc.h"
#include "context_state.h"
#include <stdlib.h>
#include <string.h>

void calc_metrics(List* list, int columnIndex, double* min, double* max, double* median) {
    *min = get_column_value((DataRow*)list->head->data, columnIndex);
    *max = get_column_value((DataRow*)list->tail->data, columnIndex);
    int mid = (list->size - 1) / 2;
    Node* midNode = list->head;
    for (int i = 0; i < mid; ++i) {
        midNode = midNode->next;
    }
    double v1 = get_column_value((DataRow*)midNode->data, columnIndex);
    if (list->size % 2 == 1) {
        *median = v1;
    } else {
        double v2 = get_column_value((DataRow*)midNode->next->data, columnIndex);
        *median = (v1 + v2) / 2.0;
    }
}

void logic_calculate_metrics(AppContext* context, const AppParams* params) {
    int success = 1;
    List* filteredList = NULL;
    YearRange years = {params->startYear, params->endYear};
    FilteredSeries series = {NULL, NULL, 0};

    if (params->startYear >= params->endYear) {
        set_status_message(context, ERROR_INVALID_PARAMS, "Invalid year range.");
        success = 0;
    }

    if (success) {
        reset_metrics(context);
        clear_plot_buffers(context);

        filteredList = filter_to_list(context->dataList, params->region, years);
        sort_list_by_column(filteredList, COLUMN_YEAR);
        list_to_series(filteredList, params->columnIndex, &series);

        context->plot.years = series.years;
        context->plot.values = series.values;
        context->plot.count = series.count;
        strncpy(context->plot.region, params->region, REGION_NAME_LENGTH - 1);
        context->plot.region[REGION_NAME_LENGTH - 1] = '\0';
        context->plot.yearMin = series.years[0];
        context->plot.yearMax = series.years[series.count - 1];

        sort_list_by_column(filteredList, params->columnIndex);
        calc_metrics(filteredList, params->columnIndex,
                     &context->metrics.min, &context->metrics.max, &context->metrics.median);
    }

    if (filteredList != NULL) {
        list_clear(filteredList);
    }

    if (success) {
        set_status_message(context, STATUS_OK, "Metrics calculated.");
    }
}
