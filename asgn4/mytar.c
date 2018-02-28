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
	char **p;
	
	if ((archive = open(paths[0], O_RDONLY)) < 0) {
		perror(paths[0]);
		exit(EXIT_FAILURE);
	}
	paths++; /* move paths forward */
	num_paths--;
	
	files = build_dir_tree(archive, s);
	
	if (num_paths == 0) {
		print_tree_helper(files, v);
	}
	
	for (i = 0; i < num_paths; i++) {
		temp_files = files;
		p = split_path(paths[i]);
		
		for (j = 0; j < path_length(p); j++) {
			while (temp_files) {
				if (strcmp(temp_files->file_name, p[j]) == 0) {
					temp_files = temp_files->child;
					break; /* stop looking @ siblings
						   and descend */
				}
				temp_files = temp_files->sibling;
			}
			if (temp_files == NULL) {
				fprintf(stderr, "nonexistant path\n");
				break;
			}
		}
		/* temp_file should now point to correct subdirectory/file */
		print_tree(temp_files, v);
	}
	
	close(archive);
	
	/* Need to free tree */
}

void create_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to create archive by reading in paths and generating headers **/
	int i, fd;
	struct stat sb;
	int archive;
	char rel_path[2048];
	
	if ((archive = open(paths[0], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0) {
		perror(paths[0]);
		exit(EXIT_FAILURE);
	}
	paths++; /* move paths forward */
	num_paths--;
	
	/* If not given any paths, exit? */
	if (0 == num_paths) {
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
			memset(rel_path, 0, strlen(rel_path));
			strcpy(rel_path, paths[i]);  
			/* Is regular file */
			if (S_ISREG(sb.st_mode)) {
				write_header(archive, rel_path, s);
			}
			/* Is directory */
			if (S_ISDIR(sb.st_mode)) {
				printf("%s\n", rel_path);
				handle_dir(archive, rel_path, paths[i], s);
			}
		}
		close(fd); 
	}
	
	close(archive);
	return;
}

void extract_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to extract from an archive, recreating the files correctly */
	int i, fd;
	tree files;
	int archive;
	
	if ((archive = open(paths[0], O_RDONLY)) < 0) {
		perror(paths[0]);
		exit(EXIT_FAILURE);
	}
	paths++; /* move paths forward */
	num_paths--;
	
	files = build_dir_tree(archive, s);
	
	/* If not given explicit paths, take care of entire archive */
	if (1 == num_paths) {
		printf("%s\n", files->th.name);
		/* Unpack entire tree */
	}
	for (i = 0; i < num_paths; i++) {
		if (!(fd = open(paths[i], O_RDONLY))) {
			perror(paths[i]);
			exit(EXIT_FAILURE);
		}
		/* Go through dir traversal and find path */
	}
	
	close(archive);
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
void handle_dir(int archive, char *rel_path, char *path, bool s) {
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
					strcat(rel_path, "/"); 
					strcat(rel_path, curr_name); 
					write_header(archive, rel_path, s);
				}
				/* Is directory */
				if (S_ISDIR(sb.st_mode)) {
					strcat(rel_path, "/"); 
					strcat(rel_path, curr_name);
					printf("%s\n", rel_path); 
					handle_dir(archive, rel_path, curr_name, s);
				}
				rel_path[strlen(rel_path) - 1 - strlen(curr_name)] = 0;
			}
			close(fd);
		}
	}
	
	chdir("..");
}

tree build_dir_tree(int archive, bool s) {
	/* This needs to build entire directory tree, and then we can traverse it */
	struct stat;
	tar_header *th;
	char full_path[255];
	int len;
	tree headers = NULL;
	
	while ((th = pack_header(archive, s)) != NULL) {
		len = (int)strlen((char *)th->prefix);
		strcpy(full_path, (char *)th->prefix);
		full_path[len] = '/';
		strcpy(full_path + len + 1, (char *)th->name);
		
		if (!valid_header(*th)) {
			fprintf(stderr, "pack_header: malformed header found\n");
			break;
		}
		
		headers = build_tree(headers, full_path, th);
	}
	
	/* keep reading headers while not hitting two null blocks */
	
	return headers;
}

tar_header *pack_header(int fd, bool s) {
	/* each header is 500 bytes */
	/* do something with ustar??? */
	int file_size;
	tar_header *th;
	uint8_t buf[512]; /* the last 12 bytes are data, not header */
	
	th = new_header();
	
	if (read(fd, buf, 512) < 512) {
		return NULL; /* end of file */
	}
	
	if (null_block(buf)) {
		return NULL;
	}
	
	
	memcpy(&th->name, buf, 100);
	memcpy(&th->mode, buf + 100, 8);
	memcpy(&th->uid, buf + 108, 8);
	memcpy(&th->gid, buf + 116, 8);
	memcpy(&th->size, buf + 124, 12);
	memcpy(&th->mtime, buf + 136, 12);
	memcpy(&th->chksum, buf + 148, 8);
	memcpy(&th->typeflag, buf + 156, 1);
	memcpy(&th->linkname, buf + 157, 100);
	memcpy(&th->magic, buf + 257, 6);
	memcpy(&th->version, buf + 263, 2);
	memcpy(&th->uname, buf + 265, 32);
	memcpy(&th->gname, buf + 297, 32);
	memcpy(&th->devmajor, buf + 329, 8);
	memcpy(&th->devminor, buf + 337, 8);
	memcpy(&th->prefix, buf + 345, 155);
	
	file_size = (int)strtol((char *)th->size, NULL, 8);
	
	if ((file_size -= 12) > 0) {
		file_size = file_size / 512 + 1;
	} else {
		file_size = 0;
	}
	
	/* maybe want to store file contents for extract? */
	lseek(fd, 512 * file_size, SEEK_CUR); /* go to next header */
	
	return th;
}

bool null_block(uint8_t *buf) {
	int i;
	
	for (i = 0; i < BLK_SIZE; i++) {
		if (buf[i] != '\0') {
			return false;
		}
	}
	return true;
}

bool valid_header(tar_header th) {
	unsigned int chksum;
	
	chksum = (unsigned int)strtol((char *)th.chksum, NULL, 8);
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

/**
 Helper for create_archive().

 @param archive archive file descriptor with write permissions
 @param path the path to add to the archive
 @param s the strict flag
 */
void write_header(int archive, char *path,  bool s) {
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
