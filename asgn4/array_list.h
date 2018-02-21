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
#include "mem.h"

typedef struct tar_header tar_header;
struct tar_header {
	uint8_t name[100]; /* NULL terminated only if NULL fits */
	uint8_t mode[8];
	uint8_t uid[8];
	uint8_t gid[8];
	uint8_t size[12];
	uint8_t mtime[12];
	uint8_t chksum[8];
	uint8_t typeflag;
	uint8_t linkname[100];  /* NULL terminated only if NULL fits */
	uint8_t magic[6]; /* "ustar" */
	uint8_t version[2]; /* "00" */
	uint8_t uname[32]; /* null terminated */
	uint8_t gname[32]; /* null terminated */
	uint8_t devmajor[8];
	uint8_t devminor[8];
	uint8_t prefix[155];  /* NULL terminated only if NULL fits */
};

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

struct tar_header *new_header(void);
void print_header(tar_header *th, bool v);

#endif /* array_list_h */
