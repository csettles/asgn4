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
		list_archive(argc - 2, argv + 2, verbose, strict);
	} else if (strchr(argv[1], 'c')) {
		create_archive(argc - 2, argv + 2, verbose, strict);
	} else if (strchr(argv[1], 'x')) {
		extract_archive(argc - 2, argv + 2, verbose, strict);
	} else {
		/* no function specified */
		fprintf(stderr,
			"usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE);
	}
}

void list_archive(int num_paths, char **paths, bool v, bool s) {
	struct passwd pd;
	array files;
	int i, j;
	char *archive = paths[0];
	
	paths = paths + 1; /* move paths forward */
	
	files = get_header(archive, s);

	for (i = 0; i < files->len; i++) {
		if (num_paths == 0) {
			print_header(*(files->list[0]), v);
			continue;
		}
		
		/* if there are paths specified... */
		for (j = 0; j < num_paths; j++) {
			/* somehow gotta check if header starts with any paths */;
		}
		
		free(files->list[0]); /* free file once done */
	}
	
	free(files);
}

void create_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to create archive by reading in paths and generating headers **/ 
	int i, fd;
	char *archive;

	/* Checks if first argument is a tar file */ 
	if (tar_checker(paths[0]) == 0) {
                archive = paths[0];
        } else {
                fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
                exit(EXIT_FAILURE);
        }
		
	paths = paths + 1; /* moves to path list */ 

	/* If not given any paths, exit? */ 
	if (1 == num_paths) {
		fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE); 
	}
	
	/* If made to here, correctly found tar file and found at least one path input */
	for (i = 0; i < num_paths; i++) {
		if (!(fd = open(path[i], O_RDONLY))) {
                	perror(path);
                	exit(EXIT_FAILURE);
        	}
		/* Going to need to handle traversal here if directory to get all children */ 
		pack_header(fd, s); 
	}
	return;
}

void extract_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to extract from an archive, recreating the files correctly */
	int i, fd;
	array files; 
	char *archive; 
	char *curr_path;

	/* Checks if first argument is a tar file */
        if (tar_checker(paths[0]) == 0) {
        	archive = paths[0];
        } else {
                fprintf(stderr, "usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
               	exit(EXIT_FAILURE);
        }

	files = get_header(archive, s);

	paths = paths + 1; /* moves to path list */ 

	/* If not given explicit paths, take care of entire archive */ 
	if (1 == num_paths) {
                unpack_header(*(files->list[0]), v)
        }
	for (i = 0; i < num_paths; i++) {
		curr_path = paths[i];
		/* Go through dir traversal and find path */  
	}
	return;
}

int tar_checker(char *path) {
	/* Used to check if a path is tar file */ 
	char *tar_check; 
        if ((tar_check = strchr(path_name, '.')) != NULL) {
                if (strcmp(tar_check, ".tar") == 0) {
                        return 0; 
       		}
        }
	return -1; 
}

void pack_header(int fd, bool s) {
	/* each header is 400 bytes */
	tar_header *th;
	uint8_t buf[400];
	
	
	th = new_header();
	
	if (read(fd, buf, 400) < 400) {
		fprintf(stderr, "malformed header\n");
		exit(EXIT_FAILURE);
	}
	
	memcpy(&th->mode, buf + 100, 8);
	memcpy(&th->uid, buf + 108, 8);
	memcpy(&th->gid, buf + 116, 8);
	memcpy(&th->size, buf + 124, 12);
	/* etc... */
	
	return;
}

array get_header(char *path, bool s) {
	int fd;
	struct stat 
	array headers;
	
	headers = new_array(10, sizeof(struct tar_header));
	
	if (!(fd = open(path, O_RDONLY))) {
		perror(path);
		exit(EXIT_FAILURE);
	}

		
	/* pack header and add to list */
	/* move forward at least 112 bytes */
	/* keep reading headers while not hitting two null blocks */
	
	return headers;
}

void unpack_header(int fd, tar_header th, bool s) {
	char buf[400];
	
	strncpy(buf, (char *)th.name, 100);
	strncpy(buf + 100, (char *)th.mode, 8);
	/* etc... */
}
