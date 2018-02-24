#include "dir_tree.h"

/*  Creates a tree node */ 
tree *create_node(char *data, int depth) {
	int string_len; 
	tree t;
	
	t = safe_malloc(sizeof(tree));
	
	string_len = strlen(data); 
	t->path = safe_malloc((string_len + 1)*sizeof(char)); 
	strcpy(t->path, data); 
	if (depth <= 0) {
		t->depth = 0; 
	} else {
		t->depth = depth; 
	}
	t->child = NULL;
	t->next = NULL; 	
	return t;
}

/* Appends a child to a given node */
tree *add_child(tree *n, char *data) {
	if (n == NULL) {
		return NULL; 
	}
	
	/* If the child already exists, need to add as sibling to child*/ 
	if ((n->child) != NULL) {
		return add_sibling(n->child, data);
	} else {
		return (n->child = create_node(data, n->depth)); 
	}
}

/* Appends a sibling */
tree *add_sibling(tree *n, char *data) {
	if (n == NULL) {
		return NULL;
	}
		
	/* Add things left to right */ 
	while ((n->next) != NULL) {
		n = n->next; 
	}
	
	return (n->next = create_node(data, n->depth)); 
}

/* DFS like traversing */ 
/* First print only prints the single element passed in */
void print_tree_init(tree *n) {
	if (n == NULL) {
		return; 			
	}
	printf("%s\n", n->path);
	
	print_tree_helper(n->child); 
}

/* Recursively print all children and their siblings */ 
void print_tree_helper(tree *n) {
	if (n == NULL) {
		return;
	}
	while (n != NULL) {
		printf("%s\n", n->path); 
		print_tree_helper(n->child); 
		n = n->next; 
	}
}
