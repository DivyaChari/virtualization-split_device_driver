#ifndef __KV_H__
#define __KV_H__

#include <stddef.h>
#include <stdlib.h>

#include "uhyve-syscalls.h"

#define MAX_MSG_SIZE  4096

int host_get(char *key, void *value, size_t *value_len);
int host_put(char *key, void *value, size_t value_len);

#endif
