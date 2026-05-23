#include "load_data.h"
#include "parser.h"
#include "context_state.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_LINE_LENGTH 1024

static void process_lines(AppContext* context, FILE* file) {
    if (context == NULL || file == NULL) {
        return;
    }
    char buffer[MAX_LINE_LENGTH];
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        DataRow record;
        context->rows.total++;
        if (parse_demography_line(buffer, &record)) {
            DataRow* newRow = (DataRow*)malloc(sizeof(DataRow));
            if (newRow != NULL) {
                *newRow = record;
                if (!list_push_back(context->dataList, newRow)) {
                    free(newRow);
                } else {
                    context->rows.valid++;
                }
            }
        }
    }
}

void load_data(AppContext* context, const AppParams* params) {
    int success = 1;
    FILE* file = NULL;
    char headerBuffer[MAX_LINE_LENGTH];
    const char* filePath = NULL;

    if (params == NULL || params->filePath[0] == '\0') {
        set_status_message(context, ERROR_INVALID_PARAMS);
        success = 0;
    }

    if (success) {
        filePath = params->filePath;
        clear_context_data(context);
        context->dataList = list_create();

        file = fopen(filePath, "r");
        if (file == NULL) {
            set_status_message(context, ERROR_FILE_OPEN);
            list_clear(context->dataList);
            context->dataList = NULL;
            success = 0;
        }
    }

    if (success && file != NULL) {
        if (fgets(headerBuffer, sizeof(headerBuffer), file) == NULL) {
            set_status_message(context, ERROR_FILE_READ);
            list_clear(context->dataList);
            context->dataList = NULL;
        } else {
            process_lines(context, file);
            set_status_message(context, STATUS_OK);
        }
        fclose(file);
    }
}
