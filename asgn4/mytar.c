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
	
	if (!is_archive(argv[2])) {
		fprintf(stderr, "%s: improper tar file\n", argv[2]);
		exit(EXIT_FAILURE);
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
	return 0;
}

/**
 Lists the files in an archive file. If paths are specified, only outputs
 information equal to or descending from those paths.

 @param num_paths the number of paths (0 means list everything)
 @param paths the path names
 @param v whether output is verbose
 @param s whether standards are strict
 */
void list_archive(int num_paths, char **paths, bool v, bool s) {
	/* struct passwd pd; */
	tree files;
	tree temp_files; 
	int i, j;
	int archive;
	char **path_components; 	
	
	if ((archive = open(paths[0], O_RDONLY))) {
		perror(paths[0]);
		exit(EXIT_FAILURE);
	}
	paths += 1; /* move paths forward */
	num_paths -= 1;
	
	files = get_headers(archive, s);

	for (i = 0; i < num_paths; i++) {
		if (num_paths == 1) {
			/* Print header for everything */ 
			continue;
		}
		
		/* if there are paths specified... */
		for (j = 0; j < num_paths; j++) {
			/* Break up path, and then traverse files */
			path_components = split_path(paths[j]);
			temp_files = files; 
			while (path_components != NULL) {
				/* go through current layer */ 
				while(temp_files->sibling != NULL) {
					/* If the path matches, go down a layer */
					if (strcmp(temp_files->file_name,path_components) == 0) {
						temp_files = temp_files->child; 
						path_components++; 
						break; 
					} else {
						temp_files = temp_files->sibling; 
					}
				}
			} 
			/* temp_files should now be at file */ 
		}
		
	        printf("%s\n", files->path);/* free file once done */
	}
	
	/* Need to free tree */	
}

void create_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to create archive by reading in paths and generating headers **/ 
	int i, fd;
	struct stat sb;
	char *archive;
	
	archive = paths[0];
	paths = paths + 1; /* move paths forward */

	/* If not given any paths, exit? */ 
	if (1 == num_paths) {
		fprintf(stderr,
			"usage: mytar [ctxvS]f tarfile [ path [ ... ] ]\n");
		exit(EXIT_FAILURE); 
	}
	
	/* If made to here, correctly found tar file and found at least one path input */
	for (i = 0; i < num_paths; i++) {
		if (!(fd = open(paths[i], O_RDONLY))) {
                	perror(paths[i]);
                	exit(EXIT_FAILURE);
        	}
		if (fstat(fd, &sb) == 0) {
			/* Is regular file */
			if (S_ISREG(sb.st_mode)) {
				write_header(archive, paths[i], s); 
			}
			/* Is directory */
			if (S_ISDIR(sb.st_mode)) {
				handle_dir(archive, paths[i], s); 
			} 
		}
	}
	return;
}

void extract_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to extract from an archive, recreating the files correctly */
	int i, fd;
	tree files;
	char *archive;
	
	archive = paths[0];
	paths = paths + 1; /* move paths forward */

	files = get_header(archive, s);

	/* If not given explicit paths, take care of entire archive */ 
	if (1 == num_paths) {
        	printf("%s\n", files->path);	
		/* Unpack entire tree */
	}
	for (i = 0; i < num_paths; i++) {
		if (!(fd = open(paths[i], O_RDONLY))) {
                        perror(paths[i]);
                        exit(EXIT_FAILURE);
                }
		/* Go through dir traversal and find path */  
	}
	return;
}

/**
 Checks whether a give file path is an archive file.

 @param path the name of the archive file
 @return whether it is an archive
 */
bool is_archive(char *path) {
	/* Used to check if a path is tar file */ 
	char *tar_check; 
        if ((tar_check = strchr(path, '.')) != NULL) {
                if (strcmp(tar_check, ".tar") == 0) {
                        return true;
       		}
        }
	return false;
}

/**
 Helper function for create_archive. Writes headers for every file in the
 directory.

 @param archive the archive file
 @param path <#path description#>
 @param s <#s description#>
 */
void handle_dir(char *archive, char *path, bool s) {
	/* For create_archive, when given a directory to archive */
	DIR *d;
	struct dirent *dir;
	struct stat sb; 

	char *curr_name; 
	int fd;
	
	/* Opens current directory */
	chdir(path); 
	d = opendir("."); 
	
	if (d != NULL) {
		while((dir = readdir(d)) != NULL) {
			curr_name = dir->d_name;
			if (!strcmp(curr_name,".") || !strcmp(curr_name,"..")) {
				continue;
			}
			if (!(fd = open(curr_name, O_RDONLY))) {
                	        perror(curr_name);
        	                exit(EXIT_FAILURE);
	                }
	                if (fstat(fd, &sb) == 0) {
        	                /* Is regular file */
                	        if (S_ISREG(sb.st_mode)) {
                        	        write_header(archive, curr_name, s);
                       		 }
                        	/* Is directory */
                        	if (S_ISDIR(sb.st_mode)) {
                                	handle_dir(archive, curr_name, s);
                        	}
			}
		}
	}

	chdir("..");	
}

void pack_header(int fd, bool s) {
	/* each header is 500 bytes */
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

tree get_header(char *path, bool s) {
	/* This needs to build entire directory tree, and then we can traverse it */ 
	int fd;
	char *curr_path; 
	unsigned char buffer_header[500]; 
	size_t bytes_read = 500; /* Might want to change this */
	struct stat; 
	tree headers = create_node(NULL, 0);
	
	if (!(fd = open(path, O_RDONLY))) {
		perror(path);
		exit(EXIT_FAILURE);
	}
	
	while ((bytesread = read(fd, buffer_header, 500)) > 0) {
		tar_header curr_header = new_header(); /* make the header */
		curr_path = curr_header.name; 
		headers = build_tree(headers, curr_path, curr_header);  	
	}

	/* pack header and add to list */
	/* move forward at least 12 bytes */
	/* keep reading headers while not hitting two null blocks */
	
	close(fd);
	
	return headers;
}

bool valid_header(tar_header th) {
	unsigned int chksum;
	
	chksum = atoi((const char *)th.chksum);
	return calc_chksum(th) == chksum;
}

int calc_chksum(tar_header th) {
	unsigned int sum = 0;
	
	sum += sum_of_string(th.name, 100);
	sum += sum_of_string(th.mode, 8);
	sum += sum_of_string(th.uid, 8);
	sum += sum_of_string(th.gid, 8);
	sum += sum_of_string(th.size, 12);
	sum += sum_of_string(th.mtime, 12);
	sum += sum_of_string((const uint8_t *)"        ", 8);
	sum += th.typeflag;
	sum += sum_of_string(th.linkname, 100);
	sum += sum_of_string(th.magic, 6);
	sum += sum_of_string(th.version, 2);
	sum += sum_of_string(th.uname, 32);
	sum += sum_of_string(th.gname, 32);
	sum += sum_of_string(th.devmajor, 8);
	sum += sum_of_string(th.devminor, 8);
	sum += sum_of_string(th.prefix, 155);
	
	return sum;
}

void write_header(char *archive, char *path,  bool s) {
	/* Used to write the header to the archive file */ 
        printf("%s\n", path); 
}

int sum_of_string(const uint8_t *s, int length) {
	int i, sum = 0;
	for (i = 0; i < length; i++) {
		/* add up string even after termination */
		sum += s[i];
	}
	return sum;
}

void unpack_header(tar_header th, bool s) {
	char buf[500];
	
	strncpy(buf, (char *)th.name, 100);
	strncpy(buf + 100, (char *)th.mode, 8);
	strncpy(buf)
	/* etc... */
}
