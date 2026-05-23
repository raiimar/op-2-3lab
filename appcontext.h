#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "data.h"
#include "list.h"
#include "status.h"

#define ERROR_MESSAGE_LENGTH 256

typedef struct {
    int total;
    int valid;
} Rows;

typedef struct {
    double min;
    double max;
    double median;
} MetricsResult;

typedef struct {
    List* filteredData;
    int columnIndex;
    char region[REGION_NAME_LENGTH];
    int yearMin;
    int yearMax;
} PlotData;

typedef struct {
    Status code;
    char message[ERROR_MESSAGE_LENGTH];
} ErrorInfo;

typedef struct AppContext {
    List* dataList;
    Rows rows;
    MetricsResult metrics;
    PlotData plot;
    ErrorInfo error;
} AppContext;

#endif
