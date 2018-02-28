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
#define BLK_SIZE 512

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <fcntl.h>

#include "array_list.h"
#include "dir_tree.h"

void list_archive(int num_paths, char **paths, bool v, bool s);
void create_archive(int num_paths, char **paths, bool v, bool s);
void extract_archive(int num_paths, char **paths, bool v, bool s);

tree build_dir_tree(int archive, bool s);
void write_header(int archive, char *path, char *rel_path, bool s, int type); 
tar_header *pack_header(int fd, bool s);
void unpack_header(tar_header th, bool s);
bool valid_header(tar_header th);

bool is_archive(char *path);
void handle_dir(int archive, char *rel_path, char *path, bool s);
int calc_chksum(tar_header th);
bool null_block(uint8_t *buf);

int sum_of_string(const uint8_t *s, int length);

#endif /* mytar_h */
