#ifndef PARSER_H
#define PARSER_H

#include "data.h"

#define FIELDS_COUNT 7

int parse_demography_line(char* line, DataRow* record);

#endif
