#include "metrics_calc.h"
#include "context_state.h"
#include "iterator.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>

int is_region_matching(const DataRow* row, const FilterCriteria* criteria) {
    int result = 0;
    if (row != NULL && criteria != NULL) {
        result = strcmp(criteria->region, row->region) == 0;
    }
    return result;
}

int is_start_year_matching(const DataRow* row, const FilterCriteria* criteria) {
    int result = 0;
    if (row != NULL && criteria != NULL) {
        result = row->year >= criteria->startYear;
    }
    return result;
}

int is_end_year_matching(const DataRow* row, const FilterCriteria* criteria) {
    int result = 0;
    if (row != NULL && criteria != NULL) {
        result = row->year <= criteria->endYear;
    }
    return result;
}

int is_row_matching(const void* filterData) {
    const FilterPredicateData* data = (const FilterPredicateData*)filterData;
    const DataRow* row = NULL;
    const FilterCriteria* criteria = NULL;
    int matches = 0;

    if (data != NULL) {
        row = (const DataRow*)data->element;
        criteria = (const FilterCriteria*)data->criterial;
        matches = is_region_matching(row, criteria) &&
                  is_start_year_matching(row, criteria) &&
                  is_end_year_matching(row, criteria);
    }
    return matches;
}

List* prepare_filtered_data(AppContext* context, const AppParams* params) {
    List* filteredList = NULL;
    if (params->calculateMetrics.startYear < params->calculateMetrics.endYear) {
        FilterCriteria criteria = {
            .region = params->calculateMetrics.region,
            .startYear = params->calculateMetrics.startYear,
            .endYear = params->calculateMetrics.endYear
        };

        filteredList = filter_to_list(context->dataList, is_row_matching, &criteria);
        if (filteredList != NULL && filteredList->size > 0) {
            sort_list_by_column(filteredList, COLUMN_YEAR);
            context->plot.filteredData = filteredList;
            strncpy(context->plot.region, params->calculateMetrics.region, REGION_NAME_LENGTH - 1);
            context->plot.region[REGION_NAME_LENGTH - 1] = '\0';
            context->plot.yearMin = ((DataRow*)list_first(filteredList))->year;
            context->plot.yearMax = ((DataRow*)list_last(filteredList))->year;
            context->plot.columnIndex = (DataColumnNumbers)params->calculateMetrics.columnIndex;
        } else {
            if (filteredList != NULL) {
                list_clear(filteredList);
                filteredList = NULL;
            }
        }
    }
    return filteredList;
}

MetricsResult calc_metrics(List* list, int columnIndex) {
    MetricsResult result;
    result.min = 0.0;
    result.max = 0.0;
    result.median = 0.0;
    Iterator it = iterator_create(list);
    DataRow* firstRow = (DataRow*)iterator_get(&it);
    result.min = result.max = get_column_value(firstRow, columnIndex);

    while (iterator_has_next(&it)) {
        DataRow* row = (DataRow*)iterator_get(&it);
        double val = get_column_value(row, columnIndex);
        if (val < result.min)
            result.min = val;
        if (val > result.max)
            result.max = val;
        iterator_next(&it);
    }

    List* sorted = copy_list(list);
    sort_list_by_column(sorted, columnIndex);

    int medianIndex = (sorted->size - 1) / 2;
    Iterator sit = iterator_create(sorted);
    for (int i = 0; i < medianIndex && iterator_has_next(&sit); ++i) {
        iterator_next(&sit);
    }
    DataRow* midRow = (DataRow*)iterator_get(&sit);
    double v1 = get_column_value(midRow, columnIndex);

    if (sorted->size % 2 == 1) {
        result.median = v1;
    } else {
        iterator_next(&sit);
        DataRow* midRow2 = (DataRow*)iterator_get(&sit);
        double v2 = get_column_value(midRow2, columnIndex);
        result.median = (v1 + v2) / 2.0;
    }

    list_clear(sorted);
    return result;
}

void logic_calculate_metrics(AppContext* context, const AppParams* params) {
    List* filteredList = NULL;
    int success = 1;

    if (params->calculateMetrics.startYear >= params->calculateMetrics.endYear) {
        set_status_message(context, ERROR_INVALID_PARAMS);
        success = 0;
    }

    if (success) {
        reset_metrics(context);
        clear_plot_buffers(context);
        filteredList = prepare_filtered_data(context, params);
        if (filteredList == NULL) {
            set_status_message(context, ERROR_EMPTY_RESULT);
            success = 0;
        }
    }

    if (success) {
        MetricsResult res = calc_metrics(filteredList, params->calculateMetrics.columnIndex);
        context->metrics = res;
        set_status_message(context, STATUS_OK);
    } else {
        if (filteredList != NULL) {
            list_clear(filteredList);
            context->plot.filteredData = NULL;
        }
    }
}
