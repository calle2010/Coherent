/*
 * Expand $SRCPATH and apply it to each argument printing the
 * full path of the result.
 * Allow pattern type arguments for example.
 * srcpath "*.[ch]"
 * Will find all .c and .h files on $SRCPATH eliminating duplicates.
 * .: will automatically be prepended to $SRCPATH so that files
 * in the current directory have preference.
 * 
 * The first arguments may be -p pathlist for example
 * srcpath -p ".:/usr/src/cmd" "*.c"
 * Note: . is not automatically prepended to this list.
 *
 * The -a option disables shadowing. Normally if a file is found
 * in two directorys on the path only the first if printed.
 * -a causes it to be printed twice.
 *
 * Normally srcpath silently bypasses directorys and matching files
 * on which it has no read permission. The -w option causes it to
 * print a warning message on stderr.
 */
#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <path.h>
#include <dirent.h>
#include <access.h>

struct savepath {
	char name[DIRSIZ + 1];	/* zero terminated directory name */
	char *path;		/* pointer to path or NULL */
};

static struct savepath *found = NULL; 
static int ents;
static int allsw, warnsw;

/*
 * Print out of space message and die.
 */
static void
outSpace()
{
	fprintf(stderr, "srcpath: out of space");
	exit(1);
}

/*
 * Print cannot read warning if warnsw is on.
 */
static void
noRead(name)
char *name;
{
	if (warnsw)
		fprintf(stderr, "srcpath: cannot read '%s'\n", name);
}

/*
 * Check if the directory entry matches any argv entries.
 */
static void
checkit(d_name, argv, path)
char *d_name;
register char *argv[];
char *path;
{
	char work[DIRSIZ + 1];	/* zero terminated version of dent->d_name */
	char name[MAXPATH];
	static int nents;	/* number of entries allowed */

	memcpy(work, d_name, DIRSIZ);
	work[DIRSIZ] = '\0';

	for (;NULL != *argv; argv++) {
		/* the filename must match the arguement */
		if (!pnmatch(work, *argv, 0))
			continue;

		/* we must have read permission */
		sprintf(name, "%s/%s", path, work);
		if(access(name, AREAD)) {
			noRead(name);
			continue;
		}

		/* realloc() more space if we've run out. */
		if(ents >= nents) {
			extern char *realloc();

			nents += 20;
			found = realloc(found, nents * sizeof(*found));
			if(NULL == found)
				outSpace();
		}

		/* save name and pointer to path */
		strcpy(found[ents].name, work);
		found[ents++].path = path;
	}
}

/*
 * Compare two directory entrys by name then location of the path pointer.
 */
static int
compr(d1, d2)
register struct savepath *d1, *d2;
{
	register int i;

	return ((i = strcmp(d1->name, d2->name)) ? i : (d1->path - d2->path));
}

main(argc, argv)
char *argv[];
{
	register char *p;
	char *end, *env, *dotenv;
	extern char *optarg, *getenv(), *strchr();
	extern int optind;
	int i, errflag;

	dotenv = NULL;
	allsw, errflag = 0;
	while ((i = getopt(argc, argv, "?awp:")) != EOF) {
		switch (i) {
		case 'a':
			allsw = 1;	/* print all matches */
			break;
		case 'w':
			warnsw = 1;	/* warn of inaccessable files */
			break;
		/* if args start -p pathlist use that pathlist not $SRCPATH */
		case 'p':
			dotenv = optarg;
			break;
		default:
			errflag++;
		}
	}
	argv += optind;	/* point argv at first file name */
	if (errflag) {
		fprintf(stderr,
			"usage: srcpath [-aw] [-p path] file ...\n");
		exit(1);
	}

	if (NULL == dotenv) {		 /* no use of -p */
		if (NULL == (env = getenv("SRCPATH"))) {
			if (NULL == (dotenv = malloc(2)))
				outSpace();
			strcpy(dotenv, ".");
		}
		else {
			/* prepend .: to $SRCPATH */
			if (NULL == (dotenv = malloc(strlen(env) + 3)))
				outSpace();
			sprintf(dotenv, ".%c%s", LISTSEP, env);
		}
	}

	/* replace LISTSEP with '\0' and locate the end */
	for (end = dotenv; *end; end++)
		if (LISTSEP == *end)
			*end = '\0';
	
	/* for each path name */
	for (p = dotenv; p < end; p = strchr(p, '\0') + 1) {
		DIR *dirp;
		struct dirent *dp;

		/* ignore bad path entries */
		if (access(p, ALIST) || (NULL == (dirp = opendir(p)))) {
			noRead(p);
			continue;
		}

		/* for each directory entry */
		while (NULL != (dp = readdir(dirp)))
			checkit(dp->d_name, argv, p);

		closedir(dirp);
	}

	/* sort by name then position on pathlist */
	qsort(found, ents, sizeof(struct savepath), compr);

	/* eliminate duplicate names and print the remainder. */
	for (i = 1; i <= ents; i++) {
		if (!allsw && !strcmp(found[i - 1].name, found[i].name))
			found[i].path = NULL;
		if (NULL != found[i - 1].path)
			printf("%s/%s\n", found[i - 1].path, found[i - 1].name);
	}
	exit(0);
}
