#include "Error.h"

const char *ERROR_STRING[] = {
    FOREACH_ERROR(GENERATE_STRING)
};

uint32_t error_flags = 0;

void setError(ErrorType error, const char* text) {
  error_flags |= (1 << error);
  DEBUG_printf(FS("ERROR: %s. %s\n"), ERROR_STRING[error], text != nullptr ? text : FS(""));
}

void resetError(ErrorType error, const char* text) {
  error_flags &= ~(1 << error);
  DEBUG_printf(FS("ERROR cleared: %s. %s\n"), ERROR_STRING[error], text != nullptr ? text : FS(""));
}
