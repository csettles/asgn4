/*
 *  mem.h
 *  asgn3
 *
 *  Created by Caitlin Settles on 02/20/18.
 *  Copyright © 2018 Caitlin Settles. All rights reserved.
 */

#ifndef mem_h
#define mem_h

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

typedef enum {false, true} bool;

void *safe_malloc(size_t size);
void *safe_calloc(size_t size, size_t size_elem);
void *safe_realloc(void *ptr, size_t size);

#endif /* mem_h */

