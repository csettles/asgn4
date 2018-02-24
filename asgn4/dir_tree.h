#ifndef dir_tree_h
#define dir_tree_h

#include <stdio.h>
#include <string.h>
#include <pwd.h>
#include <grp.h>
#include "mem.h"

typedef struct tree tree;
struct tree {
	char *path;
	struct tree *child;
	struct tree *next;
};

tree *create_node(char *data, int depth);
tree *add_child(tree *n, char *data);
tree *add_sibling(tree *n, char *data);

void print_tree_init(tree *n);
void print_tree_helper(tree *n); 

#endif 
