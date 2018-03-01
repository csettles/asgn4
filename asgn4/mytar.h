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
#include <sys/time.h>

#include "dir_tree.h"
#include "mem.h"

/* create_archive() */
void create_archive(int num_paths, char **paths, bool v, bool s);
void handle_dir(int archive, char *rel_path, char *path, bool s);
void write_header(int archive, char *path, char *rel_path, bool s, char type);
void write_entry(int archive, char *buf, char *path, size_t size, char type);
bool split_name_prefix(char *path, char *name, char *prefix);

/* extract_archive() */
void extract_archive(int num_paths, char **paths, bool v, bool s);
void extract_paths(tree n, bool v);
void make_path(tree node);

/* list_archive() */
void list_archive(int num_paths, char **paths, bool v, bool s);

/* helpers */
int sum_of_string(const uint8_t *s, int length);
bool null_block(uint8_t *buf);
int calc_chksum(tar_header th);
int calc_chksum_buf(char *buf);
bool is_archive(char *path);
bool valid_header(tar_header th);
tree build_dir_tree(int archive, bool s);
tar_header *pack_header(int fd, bool s);
void fill_with_null(int archive, int spaces_full);

#endif /* mytar_h */
