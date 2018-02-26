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
	/* write_header(paths[0]); */
	return;
}

void extract_archive(int num_paths, char **paths, bool v, bool s) {
	return;
}

void pack_header(int fd, bool s) {
	/* each header is 400 bytes */
	tar_header *th;
	uint8_t buf[512]; /* the last 12 bytes are data, not header */
	
	th = new_header();
	
	if (read(fd, buf, 512) < 512) {
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
