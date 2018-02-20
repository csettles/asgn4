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
	int opt;
	
	while ((opt = getopt(argc, argv, "ctxvS")) != -1) {
		switch (opt) {
			case 'c':
				printf("chose c\n");
				break;
			case 't':
				printf("chose t\n");
			case '?':
				if (optopt == 'n')
					fprintf(stderr,
						"usage: fw [-n num] [ file1 [ file 2 ...] ]\n");
				return 1;
			default:
				exit(EXIT_FAILURE);
		}
	}
}
