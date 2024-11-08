/*
 * $Header: /newbits/bin/mail.c,v 1.6 89/02/22 05:34:00 bin Exp $
 * $Log:	/newbits/bin/mail.c,v $
 * Revision 1.6	89/02/22  05:34:00 	bin
 * Changes by rec to integrate with lauren's uumail.
 * 
 * Revision 1.5	88/09/01  14:49:01	bin
 * Source administration: Re-install declaration of getenv. 
 * It was inserted after epstein made his copy.
 * 
 * Revision 1.4	88/09/01  14:44:49	bin
 * Mark Epsteins changes for ASKCC and for message scrolling, and interrupt
 * handling during processing.
 * 
 * Revision 1.3	88/09/01  14:27:41	bin
 * declare getenv to get rid of integer pointer pun error message.
 * 
 * Revision 1.2	88/09/01  11:02:23	bin
 * Remove extra declaration of header which had rcs stuff in it.
 * 
 * Revision 1.1	88/09/01  10:55:34	bin
 * Initial revision
 * 
 */
static	char	*rcsrev = "$Revision 1.1 $";
static	char	*rcshdr =
	"$Header: /newbits/bin/mail.c,v 1.6 89/02/22 05:34:00 bin Exp $";

/*
 * The mail command.
 * Coherent electronic postal system.
 * (NOTE: this command is written in such a way that
 * it assumed that it is setuid on execution to `root'.
 * All file accession is checked on this basis).
 * Modifications by rec january 1986 to include xmail.
 * 		 by epstein november 1987 to include CC:
 *		 by epstein november 1987 to allow ^C exit to leave you in
 *					  mail command processor
 *		 by epstein november 1987 to substitute /usr/games/fortune
 *					  for printing encrypted messages
 *		by rec february 1989 to tail to lauren weinstein's
 *			mail for alias expansion and uucp queuing.
 */

char helpmessage[] = "\
\
mail -- computer mail\n\
xmail -- secret computer mail\n\
Usage:	mail [ options ] [ user ... ]\n\
or:	xmail [ options ] user [ ... ]\n\
Options:\n\
	-f file		Print mail from 'file' instead of the default\n\
	-p		Print mail non-interactively\n\
	-q		Exit on interrupt, leaving mail unchanged\n\
	-r		Print mail in reverse order, latest first\n\
If 'user' is present, send each a mail message read from standard input.\n\
If 'xmail' is the command, use xencode to encrypt the mail messages.\n\
Mail message ends with EOF of a line containing only '.'.  Otherwise, read\n\
and print the invoking user's mailbox.\n\
\
";

char	isummary[] = "\
\
Command summary:\n\
	d		Delete current message and print the next message\n\
	m [user ...]	Mail current message to each named 'user'\n\
	p		Print current message again\n\
	q		Quit and update the mailbox\n\
	r		Reverse direction of scan\n\
	s [file ...]	Save message in each named 'file'\n\
	t [user ...]	Mail standard input to each named 'user'\n\
	w [file ...]	Save message in each named 'file' without its header\n\
	x		Exit without updating mailbox\n\
	newline		Print the next message\n\
	.		Print current message again\n\
	+		Print the next message\n\
	-		Print the previous message\n\
	EOF		Put undeleted mail back into mailbox and quit\n\
	?		Print this command summary\n\
	!command	Pass 'command' to the shell to execute\n\
If no 'file' is specified, 'mbox' in user's home directory is default.\n\
If no 'user' is specified, the invoking user is default.\n\
If the 'm', 'p', 't' commands are followed by an 'x',\n\
then the public key cryptosystem is applied to the message.\n\
\
";

#include <stdio.h>
#include <pwd.h>
#include <utmp.h>
#include <types.h>
#include <access.h>
#include <signal.h>
#include <sys/mdata.h>
#include <sys/stat.h>
#include <time.h>

#define	SPOOLDIR	"/usr/spool/mail/"
#define PUBKEYDIR	"/usr/spool/pubkey/"
#define	NARGS	64		/* Maximum # args to interactive command */
#define	NLINE	512		/* Longest line in a message */
#define	NCLINE	256		/* Length of an interactive command */

extern	char	*getenv();



int	rflag;			/* Reverse order of print */
int	qflag;			/* Exit after interrrupts */
int	pflag;			/* Print mail */
int	eflag;			/* Edit this mail */
int	callmexmail;		/* Xmail modifier present */

struct	msg {
	struct msg *m_next;		/* Link to next message */
	struct msg *m_prev;		/* Link to previous message */
	int	m_flag;			/* Flags - non-zero if deleted */
	int	m_hsize;		/* Size of header of message */
	fsize_t	m_seek;			/* Start position of message */
	fsize_t	m_end;			/* End of message */
};
struct msg	*m_first = NULL;	/* First message */
struct msg	*m_last = NULL;		/* Last message */

struct	tm	*tp;

char	iusage[] = "Bad command--type '?' for command summary\n";
char	nombox[] = "No mailbox '%s'.\n";
char	nomail[] = "No mail.\n";
char	noperm[] = "Mailbox '%s' access denied.\n";
char	nosend[] = "Can't send mail to '%s'\n";
char	moerr[] = "Cannot open mailbox '%s'\n";
char	wrerr[] = "Write error on '%s'\n";
char	toerr[] = "Cannot create temporary file\n";
char	nosave[] = "Cannot save letter in '%s'\n";
char	nopubk[] = "Can't send xmail to '%s'\n";

FILE	*mfp;				/* Mailbox stream */
int	myuid;				/* User-id of mail user */
int	mygid;				/* Group-id of mail user */
char	myname[25];			/* User name */
char	mymbox[256];			/* $HOME/mbox */
char	spoolname[50] = SPOOLDIR;
char	*mailbox = spoolname;
char	boxname[64];		/* Destination mailbox */
char	keyname[64];		/* Destination public key file name */
char	cmdname[1024];		/* Command for x{en,de}code filter */
				/* and for tail recursion to uumail */
				/* and for editor recursion */
char	header[256];		/* Message header */

char	*args[NARGS];			/* Interactive command arglist */
char	msgline[NLINE];
char	cline[NCLINE] = "+\n";

char	*temp;				/* Currently open temp file */
char	templ[] = "/tmp/mailXXXXXX";	/* Temp file name template */
char	*editname;			/* name of editor	   */
char	*askcc;				/* Ask for CC: list? (YES/NO) */

fsize_t	ftell();
char	*getlogin();
char	*mktemp();
char	*index();
char	*parent();
int	catchintr();
char	*malloc();

main(argc, argv)
char *argv[];
{
	register char *ap;

	callmexmail = (strcmp(argv[0], "xmail") == 0);
	if (signal(SIGINT, SIG_IGN) != SIG_IGN)
		signal(SIGINT, catchintr);
	if (argc>1 && *argv[1]=='-') {
		while (argc>1 && *argv[1]=='-') {
			for (ap=&argv[1][1]; *ap != '\0'; ap++)
				switch (*ap) {
				case 'f':
					if (argc < 3)
						usage();
					mailbox = argv[2];
					argv++;
					argc--;
					break;

				case 'm':
					break;

				case 'p':
					pflag++;
					break;

				case 'q':
					qflag++;
					break;

				case 'r':
					rflag++;
					break;

				default:
					usage();
				}
			argv++;
			argc--;
		}
	}
	setname();
	if (argc > 1) {
		qflag = 1;
		send(stdin, &argv[1], (fsize_t)0, (fsize_t)MAXLONG);
	} else {
		if ( ! pflag)
			callmexmail = 0;
		commands();
	}
	rmexit(0);
}

/*
 * Setup all the identities for the current user.
 */
setname()
{
	register struct passwd *pwp;
	register char *np;

	if ((np = getlogin()) == NULL) {
		myuid = getuid();
		if ((pwp = getpwuid(myuid)) == NULL)
			merr("Who are you?\n");
		np = pwp->pw_name;
	} else {
		if ((pwp = getpwnam(np)) != NULL)
			myuid = pwp->pw_uid;
	}
	mygid = pwp->pw_gid;
	strcat(spoolname, np);
	strcpy(myname, np);
	strcpy(mymbox, pwp->pw_dir);
	strcat(mymbox, "/mbox");
	mktemp(templ);

	if ((editname=getenv("EDITOR"))==NULL)
		editname = "/bin/ed";

	if ((askcc=getenv("ASKCC")) != NULL)
		if ( strcmp(askcc, "YES") || !isatty(fileno(stdin)) )
			askcc = NULL;
}

/*
 * Send the message found on
 * the file pointer to the list
 * of people (argv style) with
 * a NULL pointer at the end.
 * The message is copied to a temp-file
 * from position `start' to `end' (or EOF).
 */
send(fp, users, start, end)
FILE *fp;
register char **users;
fsize_t start, end;
{
	FILE *tfp, *xfp;
	char **getcc();

	temp = templ;
	if ((tfp = fopen(temp, "w")) != NULL) {
		fclose(tfp);
		if ((tfp = fopen(temp, "r+w")) == NULL)
			merr(toerr);
	} else
		merr(toerr);
	chown(temp, myuid, mygid);
	unlink(temp);
	temp = NULL;
	fseek(fp, start, 0);
	end -= start;

	eflag = 0;
	for (;;) {
		if (fgets(msgline, sizeof msgline, fp) == NULL)
			break;
		if (fp == stdin)
			if ((strcmp(".\n", msgline)==0))
				break;
			else if ((strcmp("?\n", msgline)==0)) {
				eflag = 1;
				break;
			}
		if (strncmp("From ", msgline, 5) == 0)
			putc('>', tfp);
		fputs(msgline, tfp);

		end -= strlen(msgline);	/* compiler bug */		
		if (end <= 0) {
			break;
		}
	}
	/*
	 * If interrupted, bug out.
	 */
	if (intcheck()) {
		fclose(tfp);
		return;
	}
	/*
	 * Now, see if user wants to edit the message
	 */
	if (eflag) {
		xfp = tfp;
		temp = templ;
		if ((tfp = fopen(temp, "wr")) == NULL)
			merr(toerr);
		chown(temp, myuid, mygid);
		mcopy(xfp, tfp, (fsize_t)0, (fsize_t)MAXLONG, 0);
		fclose(xfp);
		sprintf(cmdname, "%s %s", editname, templ);
		system(cmdname);
		unlink(temp);
		temp = NULL;
	}
	/*
	 * Otherwise if empty message, bug out.
	 */
	else if (ftell(tfp) == 0) {
		fclose(tfp);
		return;
	}
	/*
	 * Now see if a copy list is requested.
	 */
	if (askcc)
	  users = getcc(users);
	/*
	 * Now send the message.
	 */
	if (callmexmail)
	  xsend(users, tfp);
	else
	  usend(users, tfp);
}

char *subject()
{
  /* lauren's mail refuses to read a subject from standard input */
  /* without discarding the rest of the message */
  /* and refuses to accept an empty subject on the command line */
  /* so we supply a subject */
  static char *subject[] = {
#if 0
    "national security", "world peace", "elvis sighted",
    "ayatollah ups reward", "new version", "old version",
    "budget deficit", "technical support", "worker satisfaction",
    "profit sharing", "fringe benefit", "coherent software",
    "computer virus", "no subject", "forbidden",
    "to memory failure", "ive", "object", "not specified",
    "new policy", "old policy", "pass words",
#else
    " "
#endif
  };
  srand((int)time(NULL));
  return subject[rand() % (sizeof(subject)/sizeof(subject[0]))];
}

usend(users, tfp) char **users; FILE *tfp;
{
  FILE *xfp;
  strcpy(cmdname, "/usr/bin/uumail");
  while (*users) {
    strcat(cmdname, " ");
    strcat(cmdname, *users++);
  }
  strcat(cmdname, " -n -s'");
  strcat(cmdname, subject());
  strcat(cmdname, "'");
  rewind(tfp);
  if ((xfp = popen(cmdname, "w")) == NULL) {
    mmsg("Can't pipe to uumail\n");
    return;
  }
  if (mcopy(tfp, xfp, (fsize_t)0, (fsize_t)MAXLONG), 0)
    merr(wrerr, cmdname);
  pclose(xfp);
}
xsend(users, tfp) char **users; FILE *tfp;
{
	register char **ulist;
	register char *cp;
	register struct passwd *pwp;
	time_t curtime;
	FILE *xfp;

	time(&curtime);
	tp = localtime(&curtime);
	cp = asctime(tp);
	cp[strlen(cp)-1] = 0;
	sprintf(header, "From %s %s %s\n", myname, cp,
	    tzname[tp->tm_isdst ? 1 : 0]);
	if (users[0]!=NULL && users[1]!=NULL) {
		strcat(header, "(cc:");
		for (ulist = users; *ulist != NULL; ulist++) {
			strcat(header, " ");
			strcat(header, *ulist);
		}
		strcat(header, ")\n");
	}
	for (ulist = users; *ulist!=NULL; ulist++) {
		rewind(tfp);
		sprintf(boxname, "%s%s", SPOOLDIR, *ulist);
		sprintf(cmdname, "xencode %s >> %s", *ulist, boxname);
		if (index(*ulist, '!') != NULL
		 || (pwp = getpwnam(*ulist)) == NULL) {
			mmsg(nosend, *ulist);
			continue;
		}
		if (xaccess(*ulist) == 0) {
			mmsg(nopubk, *ulist);
			continue;
		}
		mlock(pwp->pw_uid);
		if ((xfp = fopen(boxname, "a")) == NULL) {
			mmsg(nosend, *ulist);
			munlock();
			continue;
		}
		chown(boxname, pwp->pw_uid, pwp->pw_gid);
		fprintf(xfp, "From xmail %s %s\n", cp,
		  tzname[tp->tm_isdst ? 1 : 0]);
		fclose(xfp);
		if ((xfp = popen(cmdname, "w")) == NULL) {
			mmsg("Can't pipe to xencode\n");
			continue;
		}
		if (fwrite(header, strlen(header), 1, xfp) != 1
		 || mcopy(tfp, xfp, (fsize_t)0, (fsize_t)MAXLONG), 0) {
			merr(wrerr, cmdname);
		}
		pclose(xfp);
		munlock();
	}
	fclose(tfp);
}

/*
 * Check access on a file.
 */
maccess(name)
char *name;
{
	struct stat sb;

	if (stat(name, &sb) < 0) {
		if (access(parent(name), ACREAT) < 0)
			return (-1);
	} else if (access(name, AWRITE) < 0)
		return (-1);
	return (0);
}

/*
 * Check enrollment for xmail.
 */
xaccess(name) char *name;
{
	struct stat sb;
	sprintf(keyname, "%s%s", PUBKEYDIR, name);
	return stat(keyname, &sb) >= 0;
}
/*
 * Find the parent directory for access permissions.
 */
char *
parent(name)
char *name;
{
	register char *cp, *xp;
	static char p[256];
	char *rindex();

	xp = rindex(name, '/');
	if (xp == NULL)
		return (".");
	if (xp == name)
		return ("/");
	if (xp - name >= 256)
		return ("");
	cp = p;
	while (name < xp)
		*cp++ = *name++;
	*cp = 0;
	return (p);
}

/*
 * Copy from the file stream `ifp' (starting at
 * position `start' and ending at `end' or EOF)
 * to the file stream `ofp' which is assumed
 * to be already correctly positioned.
 * Returns non-zero on errors.
 * intstop == 1 means stop on interrupt
 * intstop == 0 means ignore interrupt
 */
mcopy(ifp, ofp, start, end, intstop)
register FILE *ifp, *ofp;
fsize_t start, end;
{
	register int c;

	fseek(ifp, start, 0);
	end -= start;
	if (intstop)
		while (!intcheck() && end-- > 0  &&  (c = getc(ifp))!=EOF)
			putc(c, ofp);
	else
		while (end-- > 0  &&  (c = getc(ifp))!=EOF)
			putc(c, ofp);
	fflush(ofp);
	if (ferror(ofp))
		return (1);
	return (0);
}

/*
 * Process mail's interactive commands
 * for reading/deleting/saving mail.
 */
commands()
{
	register struct msg *mp;
	struct msg *dest;
	register char **fnp;
	register FILE *fp;
	fsize_t seek;

	readmail();
	mprint(mp = rflag ? m_last : m_first);
	for (;;) {
		readmail();
		intcheck();
		if ( ! pflag) {
			callmexmail = 0;
			mmsg("? ");
			if (fgets(cline, sizeof cline, stdin) == NULL) {
				if (intcheck())
					continue;
				break;
			}
		}
		switch (cline[0]) {
		case 'd':
			if (cline[1] != '\n')
				goto usage;
			mp->m_flag += 1;
			goto advance;

		case 'm':
		case 't':
			if (csplit(cline, args) == 1) {
				args[1] = myname;
				args[2] = NULL;
			}
			callmexmail = (cline[1] == 'x');
			if (cline[0] == 'm')
				send(mfp, args+1, mp->m_seek, mp->m_end);
			else
				send(stdin, args+1, 0L, (fsize_t)MAXLONG);
			break;

		case '.':
		case 'p':
			if (cline[1] == 'x') {
				callmexmail = 1;
				if (cline[2] != '\n')
					goto usage;
			} else if (cline[1] != '\n')
				goto usage;
			if (mprint(mp))
				break;
			goto advance;

		case 'q':
			if (cline[1] != '\n')
				goto usage;
			mquit();
			/* NOTREACHED */

		case 'r':
			if (cline[1] != '\n')
				goto usage;
			rflag = ! rflag;
			break;
		case 's':
		case 'w':
			if (csplit(cline, args) == 1) {
				args[1] = mymbox;
				args[2] = NULL;
			}
			seek = mp->m_seek;
			if (cline[0] == 'w')
				seek += mp->m_hsize;
			for (fnp = &args[1]; *fnp != NULL; fnp++) {
				fp = NULL;
				if (maccess(*fnp) < 0
				 || (fp = fopen(*fnp, "a")) == NULL
				 || mcopy(mfp, fp, seek, mp->m_end, 0))
					mmsg(nosave, *fnp);
				if (fp != NULL) {
					fclose(fp);
					chown(*fnp, myuid, mygid);
				}
			}
			break;

		case 'x':
			if (cline[1] != '\n')
				goto usage;
			rmexit(0);
			/* NOTREACHED */

		case '!':
			if (system(cline+1) == 127)
				mmsg("Try again\n");
			else
				mmsg("!\n");
			break;

		case '?':
			mmsg(isummary);
			break;

		case '-':
			if (cline[1] != '\n')
				goto usage;
			dest = rflag ? m_last : m_first;
			goto nextmsg;

		case '+':
			if (cline[1] != '\n')
				goto usage;
		case '\n':
		advance:
			dest = rflag ? m_first : m_last;
		nextmsg:
			do {
				if (mp == dest) {
					if (pflag)
						return;
					mmsg("No more messages.\n");
					break;
				}
				mp = (dest==m_last) ? mp->m_next : mp->m_prev;
			} while (mprint(mp) == 0);
			break;

		default:
		usage:
			mmsg(iusage);
			break;
		}
	}
	putc('\n', stderr);
	mquit();
}

/*
 * Read through the mail-box file
 * constructing list of letters.
 * On subsequent calls, append any additional mail
 * and notify the user.
 */
readmail()
{
	register struct msg *mp, *lmp;
	struct stat sb;
	static long m_last_end;
	long last;

	if (m_first == NULL) {
		if (stat(mailbox, &sb) < 0)
			merr(nombox, mailbox);
		if (sb.st_size == 0)
			merr(nomail);
		if (access(mailbox, AREAD) < 0)
			merr(noperm, mailbox);
		if ((mfp = fopen(mailbox, "r")) == NULL)
			merr(moerr, mailbox);
		mp = lmp = NULL;
		last = m_last_end = 0;
	} else {
		fstat(fileno(mfp), &sb);
		if (sb.st_size == m_last_end)
			return;
		mmsg("More mail received.\n");
		mp = lmp = m_last;
		last = mp->m_end;
		fseek(mfp, m_last_end, 0);
	}
	mlock(myuid);
	while (fgets(msgline, sizeof msgline, mfp) != NULL) {
		 if (strncmp("From ", msgline, 5) == 0) {
			mp = (struct msg *)malloc(sizeof(*mp));
			mp->m_prev = NULL;
			mp->m_next = NULL;
			mp->m_flag = 0;
			mp->m_hsize = strlen(msgline);
			mp->m_seek = ftell(mfp)-mp->m_hsize;
			if (lmp == NULL) {
				m_first = mp;
			} else {
				lmp->m_next = mp;
				lmp->m_end = last;
			}
			mp->m_prev = lmp;
			m_last = lmp = mp;
		}
		if (strcmp("\n", msgline) != 0
		 && strcmp("\1\1\n", msgline) != 0)
			last = ftell(mfp);
	}
	if (mp == NULL)
		merr("Not mailbox format '%s'\n", mailbox);
	mp->m_end = last;
	m_last_end = ftell(mfp);
	munlock();
}

/*
 * Split a command line up into
 * argv (passed) and argc (returned).
 */
int
csplit(command, args)
char *command;
char **args;
{
	register char *cp;
	register char **ap;
	register int c;

	cp = command;
	ap = args;
	for (;;) {
		while ((c = *cp)==' ' || c=='\t')
			*cp++ = '\0';
		if (*cp == '\n')
			*cp = '\0';
		if (*cp == '\0')
			break;
		*ap++ = cp;
		while ((c = *cp)!=' ' && c!='\t' && c!='\n' && c!='\0')
			cp++;
	}
	*ap = NULL;
	return (ap - args);
}

/*
 * Lock the appropriately-numbered mailbox
 * (numbered by user-number) from multiple
 * accesses. There is a (small) race here
 * which will be overlooked for now.
 */
char	*lockname;

mlock(uid)
int uid;
{
	register int fd;
	static char lock[32];

	lockname = lock;
	sprintf(lock, "/tmp/maillock%d", uid);
	while (access(lockname, 0) == 0)
		sleep(1);
	if ((fd = creat(lockname, 0)) >= 0)
		close(fd);
}

/*
 * Unlock the currently-set lock (by `mlock')
 * also called from rmexit.
 */
munlock()
{
	if (lockname != NULL)
		unlink(lockname);
	lockname = NULL;
}

/*
 * Exit, after rewriting the
 * mailbox
 */
mquit()
{
	register struct msg *mp;
	register FILE *nfp;
	struct stat sb;

	readmail();
	mlock(myuid);
	if (mailbox != spoolname && maccess(mailbox) < 0)
		merr(noperm, mailbox);
	fstat(fileno(mfp), &sb);
	signal(SIGINT, SIG_IGN);
	unlink(mailbox);
	if ((nfp = fopen(mailbox, "w")) == NULL)
		merr("Cannot re-write '%s'\n", mailbox);
	chown(mailbox, sb.st_uid, sb.st_gid);
	chmod(mailbox, sb.st_mode&0777);
	for (mp = m_first; mp != NULL; mp = mp->m_next)
		if (mp->m_flag == 0
		 && mcopy(mfp, nfp, mp->m_seek, mp->m_end, 0))
			merr(wrerr, mailbox);
	fclose(nfp);
	fclose(mfp);
	munlock();
	rmexit(0);
}

/*
 * Print the current message, given by
 * the message pointer (`mp').
 */
mprint(mp)
register struct msg *mp;
{
	FILE *xfp;
	if (mp->m_flag)
		return 0;
	if (callmexmail) {
		sprintf(cmdname, "xdecode");
		if ((xfp = popen(cmdname, "w")) == NULL) {
			return 0;
		}
		mcopy(mfp, xfp, mp->m_seek+mp->m_hsize, mp->m_end, 1);
		pclose(xfp);
	} else {
		fseek(mfp, mp->m_seek, 0);
		if (fgets(msgline, sizeof msgline, mfp) != NULL &&
		    strncmp(msgline, "From xmail", 10) == 0) {
			printf("From xmail\n");
			system("/usr/games/fortune");
			return(1);
		}
		mcopy(mfp, stdout, mp->m_seek, mp->m_end, 1);
	}
	return (1);
}

/*
 *	Get additional users to receive message (CC:)
 */

char **
getcc(users)
register char **users;
{
	register char **ulist = args;
	static	char names[NCLINE];

	mmsg("CC: ");
	if ( fgets(names, sizeof names, stdin) == NULL )
		return(users);

	while (*users != NULL)
		*ulist++ = *users++;

	csplit(names, ulist);
	return(args);
}

	
/*
 * Errors, usage, and exit removing
 * any tempfiles left around.
 */
mmsg(x, s)
char *x, *s;
{
	fprintf(stderr, x, s);
}

merr(x, s)
char *x, *s;
{
	mmsg(x, s);
	rmexit(1);
}

usage()
{
	mmsg(helpmessage);
	rmexit(1);
}

rmexit(s)
int s;
{
	if (temp != NULL)
		unlink(temp);
	munlock();
	exit(s);
}

/*
 * Catch interrupts, taking the
 * appropriate action based on
 * the `-q' option.
 */
int	intflag;		/* On when interrupt sent */

catchintr()
{
	signal(SIGINT, SIG_IGN);
	if (qflag)
		rmexit(1);
	intflag = 1;
	signal(SIGINT, catchintr);
}

intcheck()
{
	if (intflag) {
		intflag = 0;
		putc('\n', stderr);
		return (1);
	}
	return (0);
}

/*
 * Call the system to execute a command line
 * which is passed as an argument.
 * Change the user id and group id.
 */
system(line)
char *line;
{
	int status, pid;
	register wpid;
	register int (*intfun)(), (*quitfun)();

	if ((pid = fork()) < 0)
		return (-1);
	if (pid == 0) {		/* Child */
		setuid(myuid);
		setgid(mygid);
		execl("/bin/sh", "sh", "-c", line, NULL);
		exit(0177);
	}
	intfun = signal(SIGINT, SIG_IGN);
	quitfun = signal(SIGQUIT, SIG_IGN);
	while ((wpid = wait(&status))!=pid && wpid>=0)
		;
	if (wpid < 0)
		status = wpid;
	signal(SIGINT, intfun);
	signal(SIGQUIT, quitfun);
	return (status);
}
