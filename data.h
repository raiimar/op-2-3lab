#ifndef DATA_H
#define DATA_H

#define REGION_NAME_LENGTH 128

typedef struct {
    int year;
    char region[REGION_NAME_LENGTH];

    double natural_population_growth;
    double birth_rate;
    double death_rate;
    double general_demographic_weight;
    double urbanization;
} DataRow;

enum DataColumnNumbers {
    COLUMN_YEAR = 1,
    COLUMN_NATURAL_POPULATION_GROWTH = 3,
    COLUMN_BIRTH_RATE = 4,
    COLUMN_DEATH_RATE = 5,
    COLUMN_GENERAL_DEMOGRAPHIC_WEIGHT = 6,
    COLUMN_URBANIZATION = 7
};

#endif
