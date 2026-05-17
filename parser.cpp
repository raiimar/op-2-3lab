#include "parser.h"
#include <stdio.h>

int parse_demography_line(char* line, DataRow* record) {
    int isCorrect = 0;
    if (line != NULL && record != NULL) {
        int scannedFields = sscanf(line, "%d,%127[^,],%lf,%lf,%lf,%lf,%lf",
                                   &record->year,
                                   record->region,
                                   &record->natural_population_growth,
                                   &record->birth_rate,
                                   &record->death_rate,
                                   &record->general_demographic_weight,
                                   &record->urbanization);
        if (scannedFields == FIELDS_COUNT) {
            isCorrect = 1;
        }
    }
    return isCorrect;
}
