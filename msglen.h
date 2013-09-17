#pragma once

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

// Reads binary buffer and returns size of first object in buffer.
// Returns 0 if message is incomplete or on error.
// You should free error manual
size_t
msgpackclen_buf_read (void *buf, size_t size, char **error);

#ifdef __cplusplus
} // extern "C"
#endif
