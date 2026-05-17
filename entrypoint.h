#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include "appcontext.h"

typedef enum {
    OPERATION_INITIALIZATION,
    OPERATION_LOAD_DATA,
    OPERATION_CALCULATE_METRICS,
    OPERATION_CLEAR_DATA
} Operation;

typedef struct {
    char filePath[256];
    char region[128];
    int columnIndex;
    int startYear;
    int endYear;
} AppParams;

void doOperation(Operation operation, AppContext* context, AppParams* params);

#endif
