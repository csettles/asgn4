/*=============================================================================
 *   Assignment:  4 - mytar
 *
 *       Author:  Caitlin Settles
 *        Class:  CSC 357 Section 01
 *     Due Date:  02/28/18
 *
 *-----------------------------------------------------------------------------
 *
 *  Description:  Emulates the functionality of GNU's tar.
 *
 *        Input:  A tar archive or files to archive.
 *
 *       Output:  A tar archive or dearchived files.
 *
 *===========================================================================*/

#include "mytar.h"

int main(int argc, char *argv[]) {
	bool verbose, strict;
	verbose = false;
	strict = false;
	
	if (argc < 3) {
		/* incorrect number of arguments */
		fprintf(stderr,
			"usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
	
	if (!strchr(argv[1], 'f')) {
		/* no archive file specified */
		fprintf(stderr, "archive file required\n");
		exit(EXIT_FAILURE);
	}
	
	if (strchr(argv[1], 'v')) {
		verbose = true;
	}
	if (strchr(argv[1], 'S')) {
		strict = true;
	}
	
	if (strchr(argv[1], 't')) {
		list_archive(argc - 2, argv + 2);
	} else if (strchr(argv[1], 'c')) {
		create_archive(argc - 2, argv + 2);
	} else if (strchr(argv[1], 'x')) {
		extract_archive(argc - 2, argv + 2);
	} else {
		/* no function specified */
		fprintf(stderr,
			"usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
}

void list_archive(int num_paths, char **paths) {
	char *archive = paths[0];
	
	if (num_paths == 0) {
		
	}
	return;
}

void create_archive(int num_paths, char **paths) {
	/* write_header(paths[0]); */
	return;
}

void extract_archive(int num_paths, char **paths) {
	return;
}
