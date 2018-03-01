#ifndef dir_tree_h
#define dir_tree_h

#include <sys/types.h>
#include <stdint.h>
#include <sys/stat.h>
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
    char *file_name; 
    tar_header th;
    struct tree *child;
    struct tree *sibling;
};

tree create_node(char *path, tar_header *data);
tree add_child(tree n, char *path, tar_header *data);
tree add_sibling(tree n, char *path, tar_header *data);
bool is_dir(tree n);
tree find_node(tree n, char *path);

tree build_tree(tree root, char *curr_path, tar_header *th); 
int path_length(char **path_components); 
char ** split_path(char *curr_path);

void print_tree(tree n, bool v);
void print_tree_helper(tree n, bool v);

tar_header *new_header(void);
void print_header(tar_header *th, bool v);
void print_name(tar_header *th);

#endif 
