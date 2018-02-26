/*=============================================================================
 *
 *       Author:  Caitlin Settles
 *        Class:  CSC 357 Section 01
 *     Due Date:  01/26/18
 *
 *-----------------------------------------------------------------------------
 *
 *  Description:  An array list implementation composed of nodes.
 *
 *===========================================================================*/

#include "array_list.h"

/**
 Creates a new array list
 
 @param capacity the number of elements the array list can store
 @param elem_size the size of each element in the list
 @return the newly created array
 */
array new_array(int capacity, size_t elem_size) {
	array a;
	a = safe_malloc(sizeof(struct array));
	a->list = safe_calloc(capacity, elem_size);
	a->elem_size = elem_size;
	a->capacity = capacity;
	a->len = 0;
	
	return a;
}

/**
 Appends an element to the end of the array list
 
 @param a the array to add to
 @param th the element to add
 */
void array_append(array a, tar_header *th) {
	if (a->len >= a->capacity - 1) {
		a->capacity *= 2;
		a->list = safe_realloc(a->list, a->elem_size * a->capacity);
	}
	a->list[a->len] = th;
	a->len++;
}

/**
 Prints an array
 
 @param a the array to print
 */
void print_array(array a) {
	int i;
	printf("------array------\n");
	for (i = 0; i < a->len; i++) {
		printf("#%i\t", i);
		print_header(a->list[i], true);
		printf("\n");
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
	int file_mode, gid, uid, size;
	/* struct passwd pd;
	struct group gd; */
	
	if (!v) {
		printf("%s/%s\n", th->prefix, th->name);
		return;
	}
	
	/* these might fail? */
	uid = (int)strtol((const char *)th->uid, NULL, 10);
	gid = (int)strtol((const char *)th->gid, NULL, 10);
	file_mode = (int)strtol((const char *)th->mode, NULL, 8);
	size = (int)strtol((const char *)th->size, NULL, 8);
	
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
	printf("%d, %d \n", uid, gid);
	
	/* print size in bytes */
	printf("%8d", size);
	
	/* print time */
	
	/* print file name */
	
	printf("\n");
}
