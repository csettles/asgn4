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

#include "mem.h"

void list_archive(int num_paths, char **paths);
void create_archive(int num_paths, char **paths);
void extract_archive(int num_paths, char **paths);

#endif /* mytar_h */
