#include "dir_tree.h"

/**
 Creates a tree node.

 @param data the name of the file/directory to add
 @param depth the depth of the tree
 @return the newly created directory tree
 */
tree create_node(tar_header data, int depth) {
	int string_len; 
	tree t;
	
	t = safe_malloc(sizeof(struct tree));
	
	t->th = data;
	if (depth <= 0) {
		t->depth = 0; 
	} else {
		t->depth = depth; 
	}
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
 @param data the file name
 @return the newly added directory tree node
 */
tree add_child(tree n, char *data) {
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

/**
 Appends a sibling to a directory tree.

 @param n the directory to add a child to
 @param data the name of the directory or file
 @return the newly created directory tree
 */
tree add_sibling(tree n, char *data) {
	if (n == NULL) {
		return NULL;
	}
		
	/* Add things left to right */ 
	while ((n->sibling) != NULL) {
		n = n->sibling; 
	}
	
	return (n->sibling = create_node(data, n->depth)); 
}

/**
 Depth first traversal of the directory tree in which each path within is
 printed to stdout.

 @param n the tree to print
 */
void print_tree_init(tree n) {
	if (n == NULL) {
		return; 			
	}
	printf("%s\n", n->th.name);
	
	print_tree_helper(n->child); 
}

/**
 Determines if a tree node is a directory or regular file.

 @param n the tree node to test
 @return whether the node is a directory
 */
bool is_dir(tree n) {
	return n->child != NULL;
}

/**
 Recursively prints all nodes and their children.

 @param n the tree node to print
 */
void print_tree_helper(tree n) {
	if (n == NULL) {
		return;
	}
	while (n != NULL) {
		printf("%s\n", n->th.name);
		print_tree_helper(n->child); 
		n = n->sibling; 
	}
}

tar_header *new_header(void) {
	tar_header *th;
	th = safe_malloc(sizeof(struct tar_header));
	
	strncpy((char *)&th->magic, "ustar", 6);
	strncpy((char *)&th->version, "00", 2);
	
	return th;
}

void print_header(tar_header *th, bool v) {
	int file_mode, size;
	uid_t uid;
	gid_t gid;
	struct passwd *pd;
	struct group *gd;
	time_t mtime;
	char time[16];
	
	if (!v) {
		print_file(th);
		return;
	}
	
	/* these won't fail because max is 2^8 */
	uid = (int)strtol((const char *)th->uid, NULL, 10);
	gid = (int)strtol((const char *)th->gid, NULL, 10);
	file_mode = (int)strtol((const char *)th->mode, NULL, 8);
	size = (int)strtol((const char *)th->size, NULL, 8);
	mtime = strtol((const char *)th->mtime, NULL, 10);
	
	/* print permissions */
	printf((th->mode[0] == '1') ? "d" : "-");
	printf((file_mode & S_IRUSR) ? "r" : "-");
	printf((file_mode & S_IWUSR) ? "w" : "-");
	printf((file_mode & S_IXUSR) ? "x" : "-");
	printf((file_mode & S_IRGRP) ? "r" : "-");
	printf((file_mode & S_IWGRP) ? "w" : "-");
	printf((file_mode & S_IXGRP) ? "x" : "-");
	printf((file_mode & S_IROTH) ? "r" : "-");
	printf((file_mode & S_IWOTH) ? "w" : "-");
	printf((file_mode & S_IXOTH) ? "x" : "-");
	
	/*print owner/group name */
	if (!(pd = getpwuid(uid)) || !(gd = getgrgid(gid))) {
		fprintf(stderr, "owner not found\n");
		exit(EXIT_FAILURE);
	}
	
	if (strlen(pd->pw_name) >= 17) {
		printf(" %17s", pd->pw_name);
	} else {
		printf(" %-*s/%-*s", (int)strlen(pd->pw_name),
		       pd->pw_name,
		       17 - 1 - (int)strlen(pd->pw_name),
		       gd->gr_name);
	}
	
	/* print size in bytes */
	printf(" %8d", size);
	
	/* print time */
	/* assuming th->mtime is a timestamp */
	if (strftime(time, 16, "%Y-%m-%d %H:%M", localtime(&mtime)) == 0) {
		fprintf(stderr, "unrecognized time\n");
		exit(EXIT_FAILURE);
	}
	printf(" %16s", time);
	
	/* print file name */
	printf(" ");
	print_file(th);
	
	printf("\n");
}

void print_file(tar_header *th) {
	if (strlen(th->prefix) > 0) {
		printf("%s/%s", th->prefix, th->name);
	} else {
		printf("%s", th->name);
	}
}
