/*
 *  mytar.h
 *  asgn4
 *
 *  Created by Caitlin Settles on 2/19/18.
 *  Copyright © 2018 Caitlin Settles. All rights reserved.
 */

#ifndef mytar_h
#define mytar_h

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>

#include "array_list.h"

void list_archive(int num_paths, char **paths, bool v, bool s);
void create_archive(int num_paths, char **paths, bool v, bool s);
void extract_archive(int num_paths, char **paths, bool v, bool s);

array get_header(char *path, bool s);
void pack_header(int fd, bool s);
void unpack_header(int fd, tar_header th, bool s);

int tar_checker(char *path);
	 
#endif /* mytar_h */
