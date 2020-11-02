/*
 *   ESP32 Oxygen Concentrator
 *  ===========================
 * 
 * This software is provided "as is" for educational purposes only. 
 * No claims are made regarding its fitness for medical or any other purposes. 
 * The authors are not liable for any injuries, damages or financial losses.
 * 
 * Use at your own risk!
 * 
 * License: MIT https://github.com/oxycon/ProjectApollo/blob/master/LICENSE.txt
 * For more information see: https://github.com/oxycon/ProjectApollo
 */

#include "Error.h"

const char *ERROR_STRING[] = {
    FOREACH_ERROR(GENERATE_STRING)
};

uint32_t error_flags = 0;
uint32_t error_count = 0;

ErrorLog error_log[ERROR_LOG_SIZE];
size_t first_error_log = 0;
size_t last_error_log = 0;
size_t current_error_log = -1;

void push_log(ErrorType error_id, const char* text) {
  current_error_log = last_error_log;
  error_log[last_error_log].timestamp = time(nullptr);
  error_log[last_error_log].error_id = error_id;
  if (text) {
    strncpy(error_log[last_error_log].text, text, sizeof(error_log[0].text));
    error_log[last_error_log].text[sizeof(error_log[0].text)-1] = '\0';
  } else {
    error_log[last_error_log].text[0] = '\0';
  }
  last_error_log++;
  if (last_error_log >= ERROR_LOG_SIZE) {
    last_error_log = 0;
  }
  if (last_error_log == first_error_log) {
    first_error_log++;
    if (first_error_log >= ERROR_LOG_SIZE) {
      first_error_log = 0;
    }
  }
}


size_t  get_latest_display_error(size_t log_nr, char* buffer, size_t bSize) {
  assert(buffer);
  if (!error_count) { return 0; }
  int count = last_error_log >= first_error_log ? last_error_log - first_error_log : first_error_log - last_error_log + 1;
  if (log_nr >= count) { return 0; }
  int n = (int)last_error_log -1 - log_nr;
  if (n < 0) { n += ERROR_LOG_SIZE; }
  size_t i = snprintf_P(buffer, bSize, FS("%s"), ERROR_STRING[error_log[n].error_id]);
  if (error_log[n].text[0]) {
    i += snprintf_P(buffer+i, bSize-i, FS(" - %s"), error_log[n].text);
  }
  return i;
}

size_t  get_error_log(size_t log_nr, char* buffer, size_t bSize) {
  assert(buffer);
  if (!error_count) { return 0; }
  size_t n = first_error_log + log_nr;
  if (n > ERROR_LOG_SIZE) { n -= ERROR_LOG_SIZE; }
  if (n >= last_error_log)  { return 0; }
  size_t i = snprintf_P(buffer, bSize, FS("%d %2d: %s"), error_log[n].timestamp, error_log[n].error_id, ERROR_STRING[error_log[n].error_id]);
  if (error_log[n].text[0]) {
    i += snprintf_P(buffer+i, bSize-i, FS(" - %s"), error_log[n].text);
  }
  return i;
}

size_t  get_error_json(size_t log_nr, char* buffer, size_t bSize) {
  assert(buffer);
  if (!error_count) { return 0; }
  size_t n = first_error_log + log_nr;
  if (n > ERROR_LOG_SIZE) { n -= ERROR_LOG_SIZE; }
  if (n >= last_error_log)  { return 0; }
  size_t i = snprintf_P(buffer, bSize, FS("{\"ts\":%d, \"eid\":%d, \"error\":\"%s\"}"), error_log[n].timestamp, error_log[n].error_id, ERROR_STRING[error_log[n].error_id]);
  if (error_log[n].text[0]) {
    i--;
    i += snprintf_P(buffer+i, bSize-i, FS(", \"text\":\"%s\"}"), error_log[n].text);
  }
  return i;
}

void setError(ErrorType error, const char* text) {
  error_flags |= (1 << error);
  DEBUG_printf(FS("ERROR: %s. %s\n"), ERROR_STRING[error], text != nullptr ? text : FS(""));
  push_log(error, text);
  error_count++;
}

void resetError(ErrorType error, const char* text) {
  error_flags &= ~(1 << error);
  DEBUG_printf(FS("ERROR cleared: %s. %s\n"), ERROR_STRING[error], text != nullptr ? text : FS(""));
}
