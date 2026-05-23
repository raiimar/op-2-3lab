#ifndef APPCONTEXT_H
#define APPCONTEXT_H

#include "data.h"
#include "list.h"

typedef enum {
    STATUS_OK,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_INVALID_DATA,
    ERROR_INVALID_PARAMS,
    ERROR_EMPTY_RESULT
} Status;

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

typedef struct AppContext {
    List* dataList;
    Rows rows;
    MetricsResult metrics;
    PlotData plot;
    Status status;
} AppContext;

#endif
