/*
 * Permit or deny messages (e.g. write, msg, etc.)
 * to be sent to the user's terminal.
 * As these commands are setuid, execute permission
 * may be used to control use of the terminal without
 * allowing indiscriminate use by other users of the
 * terminal.
 *
 * NOTE: the terminal is assumed to be stderr.
 */

#include <stdio.h>
#include <sys/stat.h>

main(argc, argv)
char *argv[];
{
	struct stat sb;
	char type = 'c';
	char *tty;
	char *ttyname();

	if (argc == 2) {
		if (argv[1][1]=='\0' && (*argv[1]=='y' || *argv[1]=='n'))
			type = *argv[1];
		else if (strcmp(argv[1], "no")==0 || strcmp(argv[1], "yes")==0)
			type = *argv[1];
		else
			usage();
	} else if (argc != 1)
		usage();
	tty = ttyname(fileno(stderr));
	if (tty==NULL || stat(tty, &sb)<0) {
		fprintf(stderr, "Standard error not a tty\n");
		exit(2);
	}
	if (argc > 1) {
		if (type == 'n')
			sb.st_mode &= ~S_IEXEC;
		else if (type == 'y')
			sb.st_mode |= S_IEXEC;
		chmod(tty, sb.st_mode);
	} else
		printf("%s\n", sb.st_mode&S_IEXEC ? "yes" : "no");
	exit (sb.st_mode&S_IEXEC ? 0 : 1);
}

usage()
{
	fprintf(stderr, "Usage: mesg [n] [y]\n");
	exit(2);
}