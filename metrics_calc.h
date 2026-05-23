#ifndef METRICS_CALC_H
#define METRICS_CALC_H

#include "list.h"
#include "appcontext.h"
#include "entrypoint.h"

void calc_metrics(List* list, int columnIndex, double* min, double* max, double* median);
void logic_calculate_metrics(AppContext* context, const AppParams* params);

#endif
