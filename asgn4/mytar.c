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
	tree found;
	int i, archive;
	
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
		found = find_node(files, paths[i]);
		if (found == NULL) {
			fprintf(stderr, "%s: nonexistant path\n", paths[i]);
			continue;
		}
		print_tree(found, v);
	}
	
	close(archive);
	
	/* Need to free tree */
}

void create_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to create archive by reading in paths and generating headers **/
	int i, archive;
	struct stat sb;
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
		if (lstat(paths[i], &sb) == 0) {
			memset(rel_path, 0, strlen(rel_path));
			strcpy(rel_path, paths[i]);  
			/* Is regular file */
			if (S_ISREG(sb.st_mode)) {
				write_header(archive, rel_path, paths[i], s, '0');
			}
			/* Is symlink */
			if (S_ISLNK(sb.st_mode)) {
				write_header(archive, rel_path, paths[i], s, '2'); 	
			}
			/* Is directory */
			if (S_ISDIR(sb.st_mode)) {
				write_header(archive, rel_path, paths[i], s, '5'); 
				handle_dir(archive, rel_path, paths[i], s);
			}
		} else {
			perror(paths[i]);
			continue; 
		}
	}
	write(archive, "\0\0", 2); 
	close(archive);
	return;
}

void extract_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to extract from an archive, recreating the files correctly */
	int i, archive;
	tree files, curr;
	
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
		curr = find_node(files, paths[i]);
		if (curr == NULL) {
			fprintf(stderr, "%s: nonexistant path\n", paths[i]);
			continue;
		}
		extract_paths(curr, v);
	}
	
	close(archive);
	return;
}

void extract_paths(tree n, bool v) {
	tree curr;
	if (n == NULL) {
		return;
	}
	
	curr = n;
	while (curr != NULL) {
		if (v) {
			/* print file name */
			print_name(&curr->th);
		}
		make_path(curr);
		if (is_dir(curr)) {
			chdir(n->file_name); /* change directories into file */
			extract_paths(curr->child, v);
			chdir("..");
		}
		curr = curr->sibling;
	}
}

void make_path(tree node) {
	int fd;
	mode_t mode;
	time_t mtime;
	struct timeval t[2];
	
	mode = strtol((char *)node->th.mode, NULL, 8);
	mtime = strtol((char *)node->th.mtime, NULL, 8);
	
	if (mode & S_IXUSR || mode & S_IXGRP || mode & S_IXOTH) {
		/* if execute given to anyone, execute given to all per
		 assignment spec */
		mode = 0777;
	} else {
		/* otherwise, read and write permission to everyone! */
		mode = 0666;
	}
	
	if (is_dir(node)) {
		mkdir(node->file_name, mode);
	} else {
		if (!(fd = open(node->file_name,
				O_WRONLY | O_CREAT | O_TRUNC, mode))) {
			perror(node->file_name);
			exit(EXIT_FAILURE);
		}
		/* TODO: WRITE FILE DATA HERE */
		close(fd);
	}
	
	t[0].tv_sec = time(NULL);
	t[0].tv_usec = 0;
	t[1].tv_sec = mtime;
	t[1].tv_usec = 0;
	utimes(node->file_name, t); /* restore modification time */
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
	
	/* Opens current directory */
	chdir(path);
	d = opendir(".");
	
	if (d != NULL) {
		while((dir = readdir(d)) != NULL) {
			curr_name = dir->d_name;
			if (!strcmp(curr_name,".") || !strcmp(curr_name,"..")) {
				continue;
			}
			if (lstat(curr_name, &sb) == 0) {
				/* Is regular file */
				if (S_ISREG(sb.st_mode)) {
					strcat(rel_path, "/"); 
					strcat(rel_path, curr_name); 
					write_header(archive, rel_path, curr_name, s, '0');
				}
				/* Is symblink */
				if (S_ISLNK(sb.st_mode)) {
					strcat(rel_path, "/");
					strcat(rel_path, curr_name); 
					write_header(archive, rel_path, curr_name, s, '2');
				}
				/* Is directory */
				if (S_ISDIR(sb.st_mode)) {
					strcat(rel_path, "/"); 
					strcat(rel_path, curr_name);
					write_header(archive, rel_path, curr_name, s, '5');
					handle_dir(archive, rel_path, curr_name, s);
				}
				rel_path[strlen(rel_path) - 1 - strlen(curr_name)] = 0;
			}
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
	
	if (s) {
		if (strcmp((char *)th->magic, "ustar") != 0 ||
		    strncmp((char *)th->version, "00", 2) != 0) {
			fprintf(stderr, "pack_header: malformed header found\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if (!valid_header(*th)) {
		fprintf(stderr, "pack_header: malformed header found\n");
		exit(EXIT_FAILURE);
	}
	
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
void write_header(int archive, char *path, char *rel_path, bool s, char type) {
	/* Used to write the header to the archive file */
	char prefix[150];
	char name[100];
	unsigned int dev_major;
	unsigned int dev_minor;   
	int i, j, length, chkvalue;
	struct stat sb; 
	struct passwd *pw;
	struct group *gr; 
	tar_header *th; 	
	
	/* Clears out prefix and name with null values */	
	for (i = 0; i < 150; i++) {
		prefix[i] = 0;
	}
	for (i = 0; i < 100; i++) {
		name[i] = 0; 
	}
	
	/* Determines how to split name and prefix */
	length = strlen(path);
	/* If over 100 chars, need to split */
	if (length >= 100) {
		for (i = 99; i >= 0; i--) {
			if (path[i] == '/') {
				break;
			}
		}
		/* Couldn't find a split point */
		if (i < 0) {
			fprintf(stderr, "%s: File name too long, skipping.\n", path); 
			return; 
		}		
		/* i now holds the spot of the last / */
		for (j = 0; j < length; j++) {
			if (j < i) {
				prefix[j] = path[j];
			} else {
				name[j-i] = path[j]; 
			}
		}	
	/* If under 100 chars, prefix is NUL */
	} else {
		for (i = 0; i < length; i++) {
			name[i] = path[i];
		}
	}
	
	th = new_header(); 

	if (lstat(rel_path, &sb) == 0) {
		memcpy(&th->name, name, 100); 
		memcpy(&th->mode, &sb.st_mode, 8); 
		memcpy(&th->uid, &sb.st_uid, 8);
		memcpy(&th->gid, &sb.st_gid, 8); 
		memcpy(&th->size, &sb.st_size, 12);
		memcpy(&th->mtime, &sb.st_mtime, 12);
		memcpy(&th->chksum, "        ", 8); /*Gotta do check sum */
		memcpy(&th->typeflag, &type, 1); 
		if (type == '2') {
			/* link name*/
			memcpy(&th->linkname, "ustar", 100);
		} else {
			memcpy(&th->linkname, "", 100); 
		}
		memcpy(&th->magic, "ustar", 6); 
		memcpy(&th->version, "00", 2);
		
		pw = getpwuid(sb.st_uid); 
		gr = getgrgid(sb.st_gid);
	
		memcpy(&th->uname, pw->pw_name, 32);
		memcpy(&th->gname, gr->gr_name, 32);
		
		dev_major = major(sb.st_rdev);
		dev_minor = minor(sb.st_rdev); 

		memcpy(&th->devmajor, &dev_major, 8);/* Not sure about this */
		memcpy(&th->devminor, &dev_minor, 8);/* Not sure about this */ 
		memcpy(&th->prefix, prefix, 150);
	
		chkvalue = calc_chksum(*th); 
		
		memcpy(&th->chksum, &chkvalue, 8);  
		
			
	} else {
		perror(path);
		exit(EXIT_FAILURE);
	}
	write_to_archive(archive, rel_path, *th, type);
}

void write_to_archive(int archive, char *path, tar_header th, char type) {
	int file_size, fd, read_file; 
	char *buffer; 

	write(archive, th.name, 100);
        write(archive, th.mode, 8);
        write(archive, th.uid, 8);
        write(archive, th.gid, 8);
        write(archive, th.size, 12);
        write(archive, th.mtime, 12);
        write(archive, th.chksum, 8);
        write(archive, th.typeflag, 1);
        write(archive, th.linkname, 100);
        write(archive, th.magic, 6);
        write(archive, th.version, 2);
        write(archive, th.uname, 32);
        write(archive, th.gname, 32);
        write(archive, th.devmajor, 8);
        write(archive, th.devminor, 8);
        write(archive, th.prefix, 150);
		
	/* If a file */ 
	if (type == '0' || type == '\0') {
		file_size = *th.size; 
		buffer = (char*) safe_calloc(file_size, sizeof(char)); 
		if ((fd = open(path, O_RDONLY)) < 0) {
                	perror(path);
                	exit(EXIT_FAILURE);
        	}
	
		if ((read_file = read(fd, buffer, file_size)) == -1) {
			perror("Read");
			exit(EXIT_FAILURE); 
		}
		write(archive, buffer, file_size);
		fill_with_null(archive, 500 + file_size); 
		close(fd); 
		free(buffer); 
	/* If anything else, data will be 0, need to fill rest with \0 */ 
	} else {
		fill_with_null(archive, 500); 
	}
	
} 

/* Necesary to fill the 512 size blocks */ 
void fill_with_null(int archive, int total_filled) {
	int i, space_left;
		
	space_left = BLK_SIZE - (total_filled%BLK_SIZE);
	
	for (i = 0; i < space_left; i++) {
		write(archive, '\0', 1); 
	}
}

int sum_of_string(const uint8_t *s, int length) {
	int i, sum = 0;
	for (i = 0; i < length; i++) {
		/* add up string even after termination */
		sum += s[i];
	}
	return sum;
}
