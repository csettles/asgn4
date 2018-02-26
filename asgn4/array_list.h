/*
 *  array_list.h
 *  asgn2
 *
 *  Created by Caitlin Settles on 1/24/18.
 *  Copyright © 2018 Caitlin Settles. All rights reserved.
 */

#ifndef array_list_h
#define array_list_h

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <stdint.h>
#include <sys/stat.h>

#include "mem.h"
#include "dir_tree.h"

typedef struct array *array;
struct array {
	tar_header **list;
	int capacity;
	int len;
	size_t elem_size;
};

array new_array(int capacity, size_t elem_size);
void array_append(array a, tar_header *th);
void print_array(array a);

#endif /* array_list_h */
