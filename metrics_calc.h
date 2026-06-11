#ifndef METRICS_CALC_H
#define METRICS_CALC_H

#include "list.h"
#include "appcontext.h"
#include "entrypoint.h"

typedef struct {
    const char* region;
    int startYear;
    int endYear;
} FilterCriteria;

int is_row_matching(const void* element, const void* userData);
List* prepare_filtered_data(AppContext* context, const AppParams* params);
MetricsResult calc_metrics(List* list, int columnIndex);
void logic_calculate_metrics(AppContext* context, const AppParams* params);

#endif
