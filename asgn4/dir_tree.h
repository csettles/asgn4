#ifndef dir_tree_h
#define dir_tree_h

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/stat.h>
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

typedef struct tree *tree;
struct tree {
	tar_header th;
	int depth;
	struct tree *child;
	struct tree *sibling;
};

tree create_node(tar_header data, int depth);
tree add_child(tree n, char *data);
tree add_sibling(tree n, char *data);

void print_tree_init(tree n);
void print_tree_helper(tree n);

tar_header *new_header(void);
void print_header(tar_header *th, bool v);
void print_file(tar_header *th);

bool starts_with(char *pre, char *s);

#endif 
