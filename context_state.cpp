#include "context_state.h"
#include "list.h"
#include <stdlib.h>
#include <string.h>

void reset_plot_fields(AppContext* context) {
    context->plot.years = NULL;
    context->plot.values = NULL;
    context->plot.count = 0;
    context->plot.columnIndex = 0;
    context->plot.region[0] = '\0';
    context->plot.yearMin = 0;
    context->plot.yearMax = 0;
}

void init_context(AppContext* context) {
    context->dataList = list_create();
    context->rows.total = 0;
    context->rows.valid = 0;
    reset_metrics(context);
    set_status_message(context, STATUS_OK, "");
    reset_plot_fields(context);
}

void clear_plot_buffers(AppContext* context) {
    if (context->plot.years != NULL) {
        free(context->plot.years);
    }
    if (context->plot.values != NULL) {
        free(context->plot.values);
    }
    reset_plot_fields(context);
}

void reset_metrics(AppContext* context) {
    context->metrics.min = 0.0;
    context->metrics.max = 0.0;
    context->metrics.median = 0.0;
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
    set_status_message(context, STATUS_OK, "");
}

void set_status_message(AppContext* context, Status status, const char* message) {
    context->error.code = status;
    strncpy(context->error.message, message, ERROR_MESSAGE_LENGTH - 1);
    context->error.message[ERROR_MESSAGE_LENGTH - 1] = '\0';
}
