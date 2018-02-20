/*=============================================================================
 *
 *       Author:  Caitlin Settles
 *        Class:  CSC 357 Section 01
 *     Due Date:  02/28/18
 *
 *-----------------------------------------------------------------------------
 *
 *  Description:  Helper functions to ensure safe memory allocation.
 *
 *===========================================================================*/

#include "mem.h"

/**
 Safely mallocs memory by exiting the program if not successful.
 
 @param size amount of memory to allocate
 @return pointer to newly created memory
 */
void *safe_malloc(size_t size) {
	/* comes back with memory or exits program */
	
	void *new;
	new = malloc(size);
	
	if (new == NULL) {
		perror("safe_malloc()");
		exit(EXIT_FAILURE);
	}
	return new;
}

/**
 Safely callocates memory by exiting the program if not successful.
 
 @param size the size of the block of memory
 @param size_elem the size of each element
 @return pointer to newly created memory
 */
void *safe_calloc(size_t size, size_t size_elem) {
	void *new;
	new = calloc(size, size_elem);
	
	if (new == NULL) {
		perror("safe_calloc()");
		exit(EXIT_FAILURE);
	}
	return new;
}

/**
 Safely reallocates memory by exiting the program if not successful.
 
 @param ptr pointer to memory to be reallocated
 @param size amount of new memory to allocate
 @return pointer to newly created memory
 */
void *safe_realloc(void *ptr, size_t size) {
	/* comes back with new memory size or exits program */
	void *new;
	new = realloc(ptr, size);
	
	if (new == NULL) {
		perror("safe_realloc()");
		exit(EXIT_FAILURE);
	}
	return new;
}

