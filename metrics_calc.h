#ifndef METRICS_CALC_H
#define METRICS_CALC_H

#include "list.h"
#include "appcontext.h"
#include "entrypoint.h"

void copy_list_append(List* dest, DataRow* src, int* success);
List* copy_list(List* source);
void calc_metrics(List* list, int columnIndex, double* min, double* max, double* median);
void logic_calculate_metrics(AppContext* context, const AppParams* params);

#endif
