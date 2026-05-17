#ifndef STATUS_H
#define STATUS_H

typedef enum {
    STATUS_OK,
    ERROR_FILE_OPEN,
    ERROR_FILE_READ,
    ERROR_INVALID_DATA,
    ERROR_INVALID_PARAMS,
    ERROR_EMPTY_RESULT
} Status;

#endif
