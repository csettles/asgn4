#include "dir_tree.h"

/**
 Creates a tree node.
 
 @param data the name of the file/directory to add
 @param path the name of the file
 @return the newly created directory tree
 */
tree create_node(char *path, tar_header *data) {
	tree t;
	
	t = safe_malloc(sizeof(struct tree));
	
	t->file_name = safe_malloc(strlen(path) + 1);
	strcpy(t->file_name, path);
	
	t->th = *data;
	t->child = NULL;
	t->sibling = NULL;
	return t;
}

/**
 Frees a directory tree from memory.
 
 @param n the directory tree to free
 */
void free_tree(tree n) {
	tree curr;
	if (n == NULL) {
		return;
	}
	
	while (n != NULL) {
		curr = n;
		n = n->sibling;
		
		free_tree(curr->child);
		
		free(curr->th.name);
		free(curr);
	}
}

/**
 Appends a child to a given node.
 
 @param n The directory tree to extend
 @param path the file name
 @return the newly added directory tree node
 */
tree add_child(tree n, char *path, tar_header *th) {
	if (n == NULL) {
		return create_node(path, th);
	}
	
	/* If the child already exists, need to add as sibling to child*/
	if ((n->child) != NULL) {
		return add_sibling(n->child, path, th);
	} else {
		return (n->child = create_node(path, th));
	}
}

/**
 Appends a sibling to a directory tree.
 
 @param n the directory to add a child to
 @param path the name of the directory or file
 @return the newly created directory tree
 */
tree add_sibling(tree n, char *path, tar_header *th) {
	if (n == NULL) {
		return create_node(path, th);
	}
	
	/* Add things left to right */
	while ((n->sibling) != NULL) {
		n = n->sibling;
	}
	
	return (n->sibling = create_node(path, th));
}

/**
 Determines if a tree node is a directory or regular file.
 
 @param n the tree node to test
 @return whether the node is a directory
 */
bool is_dir(tree n) {
	return n->child != NULL;
}

tar_header *new_header(void) {
	tar_header *th;
	th = safe_malloc(sizeof(struct tar_header));
	
	strncpy((char *)&th->magic, "ustar", 6);
	strncpy((char *)&th->version, "00", 2);
	
	return th;
}
 
/**
 Inserts a path into the directory tree, building along the way if the path
 does not yet exist.

 @param root the root of the tree to add to
 @param curr_path the name of the path to add
 @param th the tar header (file data) to use
 @return the updated directory tree
 */
tree build_tree(tree root, char *curr_path, tar_header *th) {
	char **path_components;
	int path_size;
	tree curr, prev;
	bool found = false;
	
	path_components = split_path(curr_path);
	path_size = path_length(path_components);
	
	if (root == NULL && path_size >= 1) {
		root = create_node(*path_components, th);
		path_components++;
	}
	
	curr = prev = root;
	/* If more things, must be a sub directory */
	while(*path_components) {
		if (curr != NULL) {
			while (curr != NULL) {
				if (strcmp(curr->file_name, 
					*path_components) == 0) {
					/* Found the correct path */
					path_components++;
					prev = curr;
					curr = curr->child; /* descend  */
					found = true;
					break; /* go to new path component */
				} else {
					prev = curr;
					curr = curr->sibling;
				}
				
			}
			if (found && curr == NULL) {
				found = false;
				curr = add_child(prev, *path_components, th);
				path_components++;
			} else if (found && curr != NULL) {
				found = false;
				continue;
			} else {
				curr = add_sibling(prev, *path_components, th);
				path_components++;
			}
		} else {
			curr = add_sibling(prev, *path_components, th);
			path_components++;
		}
	}
	
	return root;
}

/**
 Finds a node in a directory tree.

 @param n the root of the directory tree to search
 @param path the path of the file node to find
 @return the found tree node or NULL if path doesn't exist
 */
tree find_node(tree n, char *path) {
	tree curr, prev;
	char **p;
	int path_len, i;
	
	curr = prev = n;
	p = split_path(path);
	path_len = path_length(p);
	
	for (i = 0; i < path_len; i++, p++) {
		if (curr != NULL) {
			while (curr) {
				if (strcmp(curr->file_name,
					   *p) == 0) {
					/* Found the correct path */
					prev = curr;
					curr = curr->child;
					break; /* descend */
				} else {
					prev = curr;
					curr = curr->sibling;
				}
			}
		} else {
			break;
		}
	}
	/* temp_file should now point to correct subdirectory/file */
	
	return prev;
}

/**
 Splits a path on the '/' character and replaces it with a null character.
 It then creates an array of points to each "part" of the path.

 @param curr_path the path to split
 @return an array of path parts
 */
char **split_path(char *curr_path) {
	char **path_parts = NULL;
	int n_words = 0;
	
	path_parts = malloc(sizeof(char) * 1);
	
	char *curr_word = strtok(curr_path, "/");
	while(curr_word) {
		path_parts = safe_realloc(path_parts
				, sizeof(char*) * ++n_words);
		path_parts[n_words-1] = curr_word;
		curr_word = strtok(NULL, "/");
		
	}
	
	path_parts = safe_realloc(path_parts, sizeof(char*) * (n_words + 1));
	path_parts[n_words] = NULL; /* null terminate array */
	return path_parts;
}

/**
 Companion to split_path(). Gets the amount of path "parts" in a given path

 @param path_components the array returned from split_path()
 @return the number of components in the path
 */
int path_length(char **path_components) {
	int i;
	for (i = 0; path_components[i] != NULL; i++) {
		;
	}
	return i;
}

/**
 Prints a tar header struct either verbosely (equivalent to doing ls -lR)
 or simply just printing the path name.

 @param th the tar header to print
 @param v the verbose option
 */
void print_header(tar_header *th, bool v) {
	int size;
	time_t mtime;
	mode_t file_mode;
	char time[17];
	
	if (!v) {
		print_name(th);
		return;
	}
	
	/* these won't fail because max is 8^8 */
	file_mode = (int)strtol((const char *)th->mode, NULL, 8);
	size = (int)strtol((const char *)th->size, NULL, 8);
	mtime = strtol((const char *)th->mtime, NULL, 8);
	
	/* print permissions */
	if (th->typeflag == '2') {
		printf("l");
	} else if (th->typeflag == '5') {
		printf("d");
	} else {
		printf("-");
	}
	printf((file_mode & S_IRUSR) ? "r" : "-");
	printf((file_mode & S_IWUSR) ? "w" : "-");
	printf((file_mode & S_IXUSR) ? "x" : "-");
	printf((file_mode & S_IRGRP) ? "r" : "-");
	printf((file_mode & S_IWGRP) ? "w" : "-");
	printf((file_mode & S_IXGRP) ? "x" : "-");
	printf((file_mode & S_IROTH) ? "r" : "-");
	printf((file_mode & S_IWOTH) ? "w" : "-");
	printf((file_mode & S_IXOTH) ? "x" : "-");
	
	if (strlen((const char *)th->uname) >= 17) {
		printf(" %17s", th->uname);
	} else {
		printf(" %-*s/%-*s", (int)strlen((char *)th->uname),
		       th->uname,
		       17 - 1 - (int)strlen((char *)th->gname),
		       th->gname);
	}
	
	/* print size in bytes */
	printf(" %8d", size);
	
	/* print time */
	/* assuming th->mtime is a timestamp */
	if (strftime(time, 17, "%Y-%m-%d %H:%M", localtime(&mtime)) == 0) {
		fprintf(stderr, "unrecognized time\n");
		exit(EXIT_FAILURE);
	}
	printf(" %16s", time);
	
	/* print file name */
	printf(" ");
	print_name(th);
}

/**
 Prints a file/directory tree. Ignores any siblings of the given node.

 @param n the tree node to print
 @param v the verbose option
 */
void print_tree(tree n, bool v) {
	if (n == NULL) {
		return;
	}
	print_header(&n->th, v);
	print_tree_helper(n->child, v);
}

/**
 Helper function for print_tree(). Prints a tree node, its children, and then
 all its siblings.

 @param n the tree node to print
 @param v the verbose option
 */
void print_tree_helper(tree n, bool v) {
	if (n == NULL) {
		return;
	}
	
	while (n) {
		print_header(&n->th, v);
		print_tree_helper(n->child, v);
		n = n->sibling;
	}
}

/**
 Prints the path name of an archived file.

 @param th the tar header to print
 */
void print_name(tar_header *th) {
	if (strlen((const char *)th->prefix) > 0) {
		printf("%s/%s", th->prefix, th->name);
	} else {
		/* For some reason his tests only print the first 100.. idk? */ 
		printf("%-*s", 100, th->name);
	}
}
