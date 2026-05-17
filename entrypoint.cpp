#include "entrypoint.h"
#include "context_state.h"
#include "load_data.h"
#include "metrics_calc.h"

void doOperation(Operation operation, AppContext* context, AppParams* params) {
    switch (operation) {
    case OPERATION_INITIALIZATION:
        init_context(context);
        break;
    case OPERATION_LOAD_DATA:
        load_data(context, params->filePath);
        break;
    case OPERATION_CALCULATE_METRICS:
        logic_calculate_metrics(context, params->region, params->columnIndex, params->startYear, params->endYear);
        break;
    case OPERATION_CLEAR_DATA:
        clear_context_data(context);
        break;
    }
}
