#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include "appcontext.h"

#define REGION_NAME_LENGTH 128
#define FILE_PATH_LENGHT 256

typedef enum {
    OPERATION_INITIALIZATION,
    OPERATION_LOAD_DATA,
    OPERATION_CALCULATE_METRICS,
    OPERATION_CLEAR_DATA
} Operation;

typedef struct {
    char filePath[FILE_PATH_LENGHT];
} LoadDataParams;

typedef struct {
    char region[REGION_NAME_LENGTH];
    int columnIndex;
    int startYear;
    int endYear;
} CalculateMetricsParams;

typedef struct {
    union {
        LoadDataParams loadData;
        CalculateMetricsParams calculateMetrics;
    };
} AppParams;

void doOperation(Operation operation, AppContext* context, AppParams* params);

#endif
