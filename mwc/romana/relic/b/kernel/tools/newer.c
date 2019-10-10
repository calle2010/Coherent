/* newer.c - exit with status 1 if first arg is newer than second,
     else exit with status 0
   -v option gives verbose response to stdout
 */

#include <fcntl.h>
#include <stdio.h>
#include <sys/stat.h>

main(argc, argv)
int argc;
char * argv[];
{
	int fd1, fd2;
	int argn, verbose = 0;
	struct stat s1, s2;

	if (argc < 3 || argc > 4) {
		fprintf(stderr, "Usage: %s [-v] file1 file2\n", argv[0]);
		exit(2);
	}
	argn = 1;

	if (argc == 4) {
		if (strcmp(argv[1], "-v")) {
			fprintf(stderr, "Usage: %s [-v] file1 file2\n", argv[0]);
			exit(2);
		} else {
			verbose = 1;
			argn++;
		}
	}

	if ((fd1 = open(argv[argn], O_RDONLY)) < 0) {
		if (verbose)
			fprintf(stderr, "%s: can't open file %s\n",
			  argv[0], argv[argn]);
		exit(2);
	}

	if (fstat(fd1, &s1) < 0) {
		if (verbose)
			fprintf(stderr, "%s: can't stat file %s\n",
			  argv[0], argv[argn]);
		exit(2);
	}

	argn++;
	if ((fd2 = open(argv[argn], O_RDONLY)) < 0) {
		if (verbose)
			fprintf(stderr, "%s: can't open file %s\n",
			  argv[0], argv[argn]);
		exit(2);
	}

	if (fstat(fd2, &s2) < 0) {
		if (verbose)
			fprintf(stderr, "%s: can't stat file %s\n",
			  argv[0], argv[argn]);
		exit(2);
	}

	close(fd1);
	close(fd2);

	if (s1.st_mtime > s2.st_mtime) {
		if (verbose)
			printf("%s is newer than %s\n",
			  argv[argn-1], argv[argn]);
		exit (0);
	} else {
		if (verbose)
			printf("%s is NOT newer than %s\n",
			  argv[argn-1], argv[argn]);
		exit (1);
	}
}
