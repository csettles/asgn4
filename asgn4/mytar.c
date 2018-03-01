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

/**
 Creates an archive file from the given paths.
 
 @param num_paths The number of paths to create the archive from
 @param paths the paths to archive
 @param v the verbose option
 @param s the strict option (no interoperability with GNU's tar)
 */
void create_archive(int num_paths, char **paths, bool v, bool s) {
	/* Used to create archive by reading in paths and generating headers **/
	int i, archive;
	struct stat sb;
	char rel_path[2048];
	
	if ((archive = open(paths[0], O_WRONLY | O_CREAT | O_TRUNC, 0644)) < 0){
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
	
	/* If made to here, correctly found tar file and one path input */
	for (i = 0; i < num_paths; i++) {
		if (lstat(paths[i], &sb) == -1) {
			/* path does not exist */
			perror(paths[i]);
			continue;
		}
		
		memset(rel_path, 0, 2048); /* clear the buffer */
		strcpy(rel_path, paths[i]);
		if (S_ISREG(sb.st_mode)) {
			write_header(archive, rel_path,
				     paths[i], s, '0');
		} else if (S_ISLNK(sb.st_mode)) {
			write_header(archive, rel_path,
				     paths[i], s, '2');
		} else if (S_ISDIR(sb.st_mode)) {
			write_header(archive, rel_path,
				     paths[i], s, '5');
			handle_dir(archive, rel_path,
				   paths[i], s);
		}
	}
	
	memset(rel_path, 0, 1024);
	write(archive, rel_path, 1024); /* two 512-byte null blocks */
	
	close(archive);
	
	return;
}

/**
 Extracts specified paths from an archive file, or all paths if none are
 specified.
 
 @param num_paths the number of paths to extract
 @param paths the paths to extract
 @param v the verbose option
 @param s the strict option
 */
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
	if (0 == num_paths) {
		extract_paths(files, v);
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

/**
 Traverses a directory tree and makes each path in the tree. Does a depth-first
 traversal.
 
 @param n the tree from which to create files
 @param v the verbose option
 */
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
			printf("\n");
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

/**
 Makes a path from a tree node - can either be a link, directory, or file.
 
 @param node the node to "make" on the filesystem
 */
void make_path(tree node) {
	int fd, file_size;
	char *buffer;
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
		file_size = (int)strtol((char *)node->th.size, NULL, 8);
		buffer = (char*) safe_calloc(file_size * sizeof(char), sizeof(char));
		
		/* file_content null here? */
		strcpy(buffer, (char *)node->th.file_content);
		write(fd, buffer, file_size);
		close(fd);
		free(buffer);
		/* write link name if link */
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
 @param path the relative path to write
 @param s the strict option
 */
void handle_dir(int archive, char *rel_path, char *path, bool s) {
	/* For create_archive, when given a directory to archive */
	DIR *d;
	struct dirent *dir;
	struct stat sb;
	
	int length_rel;
	int length_curr;
	char *curr_name;
	
	/* Opens current directory */
	chdir(path);
	d = opendir(".");
	
	if (d != NULL) {
		while((dir = readdir(d)) != NULL) {
			curr_name = dir->d_name;
			if (!strcmp(curr_name,".")
			    || !strcmp(curr_name,"..")) {
				continue;
			}
			if (lstat(curr_name, &sb) == 0) {
				/* Is regular file */
				if (S_ISREG(sb.st_mode)) {
					strcat(rel_path, "/");
					strcat(rel_path, curr_name);
					write_header(archive, rel_path,
						     curr_name, s, '0');
				}
				/* Is symblink */
				if (S_ISLNK(sb.st_mode)) {
					strcat(rel_path, "/");
					strcat(rel_path, curr_name);
					write_header(archive, rel_path,
						     curr_name, s, '2');
				}
				/* Is directory */
				if (S_ISDIR(sb.st_mode)) {
					strcat(rel_path, "/");
					strcat(rel_path, curr_name);
					write_header(archive, rel_path,
						     curr_name, s, '5');
					handle_dir(archive, rel_path,
						   curr_name, s);
				}
				length_rel = strlen(rel_path);
				length_curr = strlen(curr_name);
				rel_path[length_rel - 1 - length_curr] = 0;
			}
		}
	}
	
	chdir("..");
}

/**
 Builds a directory tree from an archive file. Adds each file node sequentially
 found in the file.
 
 @param archive the archive file descriptor
 @param s the strict option
 @return the fully built directory tree
 */
tree build_dir_tree(int archive, bool s) {
	/* Needs to build directory tree, and then we can traverse it */
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

/**
 Helper for build_dir_tree(). Reads 512 bytes from the file, builds 500 of those
 bytes into a tar_header struct, and then lseeks to the next header block.
 Also checks for header validity.
 
 @param fd the archive file from which to build headers
 @param s the strict option
 @return the newly created tar_header
 */
tar_header *pack_header(int fd, bool s) {
	/* each header is 500 bytes */
	/* do something with ustar??? */
	int file_size, offset;
	tar_header *th;
	char *temp_content;
	uint8_t buf[512]; /* last 12 bytes are null */
	
	th = new_header();
	
	if (read(fd, buf, 512) < 512) {
		return NULL; /* end of file */
	} else if (null_block(buf)) {
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
		/* strict flag checking */
		if (strcmp((char *)th->magic, "ustar") != 0 ||
		    strncmp((char *)th->version, "00", 2) != 0) {
			fprintf(stderr,
				"pack_header: malformed header found\n");
			exit(EXIT_FAILURE);
		}
	}
	
	if (!valid_header(*th)) {
		fprintf(stderr, "pack_header: malformed header found\n");
		exit(EXIT_FAILURE);
	}
	
	file_size = (int)strtol((char *)th->size, NULL, 8);
	
	temp_content = (char *)safe_calloc(file_size, sizeof(char));
//	lseek(fd, 512, SEEK_SET);
	
	if (read(fd, temp_content, file_size) == file_size) {
		printf("%s\n", temp_content);
		/* temp_content has the CORRECT content here */
		memcpy(&th->file_content, temp_content, file_size);
	} else {
		perror("Read");
		exit(EXIT_FAILURE);
	}
	
	free(temp_content);
	
	/* round up to multiple of 512 */
	offset = file_size + BLK_SIZE - file_size % BLK_SIZE;
	
	lseek(fd, offset, SEEK_CUR); /* go to next header */
	
	return th;
}

/**
 Determines if a "block" (512 bytes) is null, which signifies the end of
 an archive file.
 
 @param buf the buffer of 512 bytes
 @return whether the entire buffer is null or not
 */
bool null_block(uint8_t *buf) {
	int i;
	
	for (i = 0; i < BLK_SIZE; i++) {
		if (buf[i] != '\0') {
			return false;
		}
	}
	return true;
}

/**
 Calculates the checksum of the header and compares against its current header.
 
 @param th the tar header to determine if valid
 @return whether header is valid
 */
bool valid_header(tar_header th) {
	unsigned int chksum;
	
	chksum = (unsigned int)strtol((char *)th.chksum, NULL, 8);
	return calc_chksum(th) == chksum;
}

/**
 Adds up every field in the header, treating all bytes as unsigned and the
 chksum field itself as 8 spaces.
 
 @param th the tar_header add up
 @return the sum of every byte in the header
 */
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

int calc_chksum_buf(char *buf) {
	/* assumes that chksum is filled with spaces already */
	int i, sum = 0;
	for (i = 0; i < BLK_SIZE; i++) {
		sum += (uint8_t)buf[i];
	}
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
	char *prefix, *name, *lname, *working_buf, *header;
	unsigned int dev_major;
	unsigned int dev_minor;
	int chkvalue;
	struct stat sb;
	struct passwd *pw;
	struct group *gr;
	
	prefix = safe_calloc(155, sizeof(char));
	name = safe_calloc(100, sizeof(char));
	header = safe_calloc(512, sizeof(char));
	lname = safe_calloc(100, sizeof(char));
	working_buf = safe_calloc(32, sizeof(char));
	
	if (!split_name_prefix(path, name, prefix)) {
		/* if file path is too long to fit into name/prefix, skip it */
		return;
	}
	
	if (lstat(rel_path, &sb) == -1) {
		/* path does not exist */
		perror(rel_path);
		return;
	}
	
	if (s) {
		printf("%s\n", rel_path);
	}
	
	/* name */
	memcpy(header, name, 100);
	
	/* mode */
	snprintf(working_buf, 8, "%o", sb.st_mode);
	memcpy(header + 100, working_buf, 8);
	memset(working_buf, 0, 32);
	
	/* uid */
	snprintf(working_buf, 8, "%o", sb.st_uid);
	memcpy(header + 108, working_buf, 8);
	memset(working_buf, 0, 32);
	
	/* gid */
	snprintf(working_buf, 8, "%o", sb.st_gid);
	memcpy(header + 116, working_buf, 8);
	memset(working_buf, 0, 32);
	
	/* size */
	snprintf(working_buf, 12, "%llo", sb.st_size);
	memcpy(header + 124, working_buf, 12);
	memset(working_buf, 0, 32);
	
	/* mtime */
	snprintf(working_buf, 12, "%lo", sb.st_mtime);
	memcpy(header + 136, working_buf, 12);
	memset(working_buf, 0, 32);
	
	/* type */
	memcpy(header + 156, &type, 1);
	
	/* linkname */
	if (type == '2') {
		readlink(path, lname, 100);
	}
	memcpy(header + 157, lname, 100);
	
	/* magic & version */
	memcpy(header + 257, "ustar", 6);
	memcpy(header + 263, "00", 2);
	
	pw = getpwuid(sb.st_uid);
	gr = getgrgid(sb.st_gid);
	
	/* uname */
	snprintf(working_buf, 32, "%s", pw->pw_name);
	memcpy(header + 265, working_buf, 32);
	memset(working_buf, 0, 32);
	
	/* gname */
	snprintf(working_buf, 32, "%s", gr->gr_name);
	memcpy(header + 297, working_buf, 32);
	memset(working_buf, 0, 32);
	
	dev_major = major(sb.st_rdev);
	dev_minor = minor(sb.st_rdev);
	
	/* dev major */
	snprintf(working_buf, 8, "%o", dev_major);
	memcpy(header + 329, working_buf, 8);
	memset(working_buf, 0, 32);
	
	/* dev minor */
	snprintf(working_buf, 8, "%o", dev_minor);
	memcpy(header + 337, working_buf, 8);
	memset(working_buf, 0, 32);
	
	/* prefix */
	memcpy(header + 345, prefix, 155);
	
	
	/* chksum */
	memcpy(header + 148, "        ", 8); /* fill check sum w/ spaces */
	chkvalue = calc_chksum_buf(header); /* this needs to be in octal */
	
	snprintf(working_buf, 8, "%o", chkvalue);
	memcpy(header + 148, working_buf, 8);
	
	write_entry(archive, header, rel_path, sb.st_size, type);
	
	free(header);
	free(name);
	free(prefix);
	free(working_buf);
}

void write_entry(int archive, char *buf, char *path, size_t size, char type) {
	int fd;
	size_t num_bytes;
	char *contents;
	
	write(archive, buf, BLK_SIZE);
	
	if (type == '0' || type == '\0') {
		if (size % BLK_SIZE != 0) {
			num_bytes = size + (BLK_SIZE - size % BLK_SIZE);
		} else {
			num_bytes = size;
		}
		
		contents = safe_calloc(num_bytes, sizeof(char));
		if ((fd = open(path, O_RDONLY)) < 0) {
			perror("write_entry");
			exit(EXIT_FAILURE);
		}
		
		if (read(fd, contents, num_bytes) < size) {
			fprintf(stderr, "file contents too short.\n");
			exit(EXIT_FAILURE);
		}
		
		write(archive, contents, num_bytes);
		close(fd);
		free(contents);
	}
}

bool split_name_prefix(char *path, char *name, char *prefix) {
	int i, j, length;
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
			fprintf(stderr,"%s: File name too long, skipping.\n",
				path);
			return false;
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
		memcpy(name, path, length + 1);
	}
	return true;
}

/**
 Helper for calc_chksum(). Adds up all the bytes in a string.
 
 @param s the string to sum
 @param length the length (# of bytes) to add
 @return the sum of all bytes in the string
 */
int sum_of_string(const uint8_t *s, int length) {
	int i, sum = 0;
	for (i = 0; i < length; i++) {
		/* add up string even after termination */
		sum += s[i];
	}
	return sum;
}
