/*
 * Wall.
 */
#include <stdio.h>
#include <utmp.h>

#define	NMSG	512

char	cant[]	= "Cannot send to %.*s on %.*s.\n";

char	msg[NMSG];
char	*ttyname();
int	nmsg;

main(argc, argv)
char *argv[];
{
	register char *mytty;
	register tfd, ufd;
	struct utmp utmp;
	char tfn[5+sizeof(utmp.ut_line)+1];

	getmsg();
	if ((ufd = open("/etc/utmp", 0)) < 0) {
		fprintf(stderr, "Cannot open utmp.\n");
		exit(1);
	}
	mytty = ttyname(fileno(stderr));
	while (read(ufd, &utmp, sizeof(utmp)) == sizeof(utmp)) {
		if (utmp.ut_line[0] == '\0')
			continue;
		sprintf(tfn, "/dev/%.*s", sizeof(utmp.ut_line), utmp.ut_line);
		if (mytty!=NULL && strcmp(mytty, tfn)==0)
			continue;
		if ((tfd = open(tfn, 1)) < 0) {
			fprintf(stderr, cant, sizeof(utmp.ut_name),
			   utmp.ut_name, sizeof(utmp.ut_line), utmp.ut_line);
			continue;
		}
		write(tfd, msg, nmsg);
		close(tfd);
	}
	close(ufd);
}

getmsg()
{
	register char *p1, *p2;
	register c;

	p1 = msg;
	p2 = "\007Broadcast message .....\n";
	while ((c = *p2++) != '\0')
		*p1++ = c;
	while ((c = getchar()) != EOF) {
		if (p1 >= &msg[NMSG]) {
			fprintf(stderr, "Message too long.\n");
			exit(1);
		}
		*p1++ = c;
	}
	nmsg = p1-msg;
}
