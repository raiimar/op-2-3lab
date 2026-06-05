#ifndef CONTEXT_STATE_H
#define CONTEXT_STATE_H

#include "appcontext.h"

void reset_plot_fields(AppContext* context);
void init_context(AppContext* context);
void clear_context_data(AppContext* context);
void reset_metrics(AppContext* context);
void clear_plot_buffers(AppContext* context);
void set_status_message(AppContext* context, Status status);

#endif
