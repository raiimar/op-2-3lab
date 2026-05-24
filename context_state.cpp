#include "context_state.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>

void reset_metrics(AppContext* context) {
    context->metrics.min = 0.0;
    context->metrics.max = 0.0;
    context->metrics.median = 0.0;
}

void set_status_message(AppContext* context, Status status) {
    context->status = status;
}

void reset_plot_fields(AppContext* context) {
    context->plot.filteredData = NULL;
    context->plot.columnIndex = COLUMN_YEAR;
    context->plot.region[0] = '\0';
    context->plot.yearMin = 0;
    context->plot.yearMax = 0;
}

void init_context(AppContext* context) {
    context->dataList = list_create();
    context->rows.total = 0;
    context->rows.valid = 0;
    reset_metrics(context);
    set_status_message(context, STATUS_OK);
    reset_plot_fields(context);
}

void clear_plot_buffers(AppContext* context) {
    if (context->plot.filteredData != NULL) {
        list_clear(context->plot.filteredData);
        context->plot.filteredData = NULL;
    }
    reset_plot_fields(context);
}

void clear_context_data(AppContext* context) {
    if (context->dataList != NULL) {
        list_clear(context->dataList);
        context->dataList = NULL;
    }
    clear_plot_buffers(context);
    reset_metrics(context);
    context->rows.total = 0;
    context->rows.valid = 0;
    set_status_message(context, STATUS_OK);
}

const char* get_status_string(Status status) {
    const char* result = "Unknown error.";
    switch (status) {
    case STATUS_OK:
        result = "Success";
        break;
    case ERROR_FILE_OPEN:
        result = "Failed to open file.";
        break;
    case ERROR_FILE_READ:
        result = "Failed to read header.";
        break;
    case ERROR_INVALID_DATA:
        result = "Invalid data in file.";
        break;
    case ERROR_INVALID_PARAMS:
        result = "Invalid parameters provided.";
        break;
    case ERROR_EMPTY_RESULT:
        result = "No data for given region and years.";
        break;
    default:
        break;
    }
    return result;
}
