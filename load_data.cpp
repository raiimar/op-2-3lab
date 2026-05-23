#include "load_data.h"
#include "parser.h"
#include "context_state.h"
#include <stdio.h>
#include <stdlib.h>
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

void load_data(AppContext* context, const char* filePath) {
    int success = 1;
    FILE* file = NULL;
    char headerBuffer[MAX_LINE_LENGTH];

    if (filePath == NULL || filePath[0] == '\0') {
        set_status_message(context, ERROR_INVALID_PARAMS, "Invalid file path.");
        success = 0;
    }

    if (success) {
        clear_context_data(context);
        context->dataList = list_create();

        file = fopen(filePath, "r");
        if (file == NULL) {
            set_status_message(context, ERROR_FILE_OPEN, "Failed to open file.");
            list_clear(context->dataList);
            context->dataList = NULL;
            success = 0;
        }
    }

    if (success && file != NULL) {
        if (fgets(headerBuffer, sizeof(headerBuffer), file) == NULL) {
            set_status_message(context, ERROR_FILE_READ, "Failed to read header.");
            list_clear(context->dataList);
            context->dataList = NULL;
        } else {
            process_lines(context, file);
            set_status_message(context, STATUS_OK, "Data loaded successfully.");
        }
        fclose(file);
    }
}
