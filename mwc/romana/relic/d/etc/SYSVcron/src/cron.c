/* 
 * 	The information contained herein is a trade secret of Mark Williams
 * 	Company, and  is confidential information.  It is provided  under a
 * 	license agreement,  and may be  copied or disclosed  only under the
 * 	terms of  that agreement.  Any  reproduction or disclosure  of this
 * 	material without the express written authorization of Mark Williams
 * 	Company or persuant to the license agreement is unlawful.
 *
 * 	An unpublished work by Mark Williams Company, Chicago.
 * 	All rights reserved.
 */

/* cron.c
 *
 * Execute commands stored in /usr/lib/crontab if it exist (3.2.0 compatibility)
 * as daemon.
 * Otherwise execute crontabs from /usr/spool/cron/crontabs.
 * It sets uid and gid to user which crontab is going to be executed.
 *
 * $ 12-10-1991 vlad (Vladimir Smelyansky)
 *
 */
#include <stdio.h>
#include <time.h>
#include <signal.h>
#include <errno.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/types.h>
#include <dirent.h>
#include "cron.h"

/*
 * Time field types.
 */
#define	MIN	0
#define HOUR	1
#define	MDAY	2
#define	MON	3
#define WDAY	4

/*
 * (Finite) States in valid().
 */
#define START	0
#define INT	1
#define INTDASH	2
#define RANGE	3
#define STR	4
#define STRPLUS	5
#define ERR	6
#define END	7
#define GLOB	('*')

/*
 * Tokens returned by gettoken(). Tokens are integers. EOF is also a token.
 * INT, STR and GLOB above are also tokens.
 */
#define DASH	('-')
#define COMMA	(',')
#define WS	(' ')
#define PERCENT	(-2)
#define NEWLINE (-3)

extern DIR		*opendir();
extern struct dirent	*readdir();
extern char		*realloc();

extern int	set_uid();	/* Set UID */
extern FILE	*fpOpenTable();	/* Open crintable */
extern int	lock();		/* Write a lock file */
extern FILE 	*cronpipe();	/* Popen for cron */

extern void	mail_entry();	/* Send mail to user if command failed */
extern child_id	*add_entry(),	/* Add an new entry to a link list */
		*find_entry(),	/* Find an entry in the link list */
		*del_entry();	/* Remove an entry from the link list */

int ifmail();

child_id	*current;	/* Pointer to the current structure */
char	acRealUser[MAX_UNAME];	/* Real user name */

int	mailFlag = TRUE;
int	flag320 = FALSE;
int	logflag;

struct	tm *tm;
time_t	clock;
int	tmfield[5];
FILE	*f;
extern int	errno;

int	ugtokflag = FALSE;
int	ugtoken;
int	ufetflag = FALSE;
int	ufetval;
int	set_uid_flag = FALSE;

char	*tokbuf;
int	buflen = MAX_STR_LEN;
char	crontab[] = "/usr/lib/crontab";
DIR	*dirp = NULL;
struct dirent	*dp;

main()
{
	int 	n;
	int	child_pid;

	sigsetup();

	tokbuf = malloc(buflen);
	/* Check if cron should be in COHERENT 3.2.0 mode */
	if ((f = fopen(crontab, "r")) != NULL) {
		fclose(f);
		chdir("/bin");	/* Under 3.2.0 cron started from /bin. */
		flag320 = TRUE;
	} else  /* Check if cron was fired. Do it only for SV cron. */
		if (lock(F_LOCK) == FALSE) {
			fprintf(stderr, "cron: locked.\n");
			exit(1);
		}

#if !DEBUG
	/* Disassociate from controling terminal and process
	 * group. Close all open files.
	 */
	bedaemon();
#endif			
	/* Open spool directory. SV mode  */
	if (flag320 == FALSE) {
		sleep(10);	/* Allows to mount all */
		if ((dirp = opendir(D_SPOOL)) == NULL) {
			system("echo cron: cannot open spool directory"
			   " > /dev/console");
			exit(1);
		}
		if (logflag = if_log()) 
			if (freopen(F_LOG, "a", stderr) == NULL) {
				fprintf(stderr, 
				   "cron: cannot open log file %s\n", F_LOG);
				exit(1);
			}
	}
			
	time(&clock);
	tm = localtime(&clock);
	alarm(61 - tm->tm_sec);

	Dprint("Time is %s\n", ctime(&clock));
	for (;;) {
		/* Put the time values into tmfield[] for easy reference.
		 * localtime() gives tm_mon in the range 0-11, tm_wday, 0-6
		 * (0=sunday). These are adjusted for cron's syntax.
		 */
		tmfield[MIN]	= tm->tm_min;
		tmfield[HOUR]	= tm->tm_hour;
		tmfield[MDAY]	= tm->tm_mday;
		tmfield[MON]	= tm->tm_mon + 1;
		tmfield[WDAY]	= tm->tm_wday;

		if (flag320 == TRUE) {		/* Run 320 mode */
			if ((f = fopen(crontab, "r")) != NULL) {
				Dprint("File pointer is 0x%x\n", f);
				strcpy(acRealUser, DAEMON);
				while (tex() != EOF)
					;
				fclose(f);
				while (wait(&n) != -1) 
					;
				if (errno == ECHILD)
					pause();
			} else
				fprintf(stderr, "cron: cannot open %s\n", 
						crontab);
		} else {			/* Run SV mode */
			while ((dp = readdir(dirp)) != NULL) {
				char	Dbuf[80];

				/* Skip '.' and '..' */
				if (!strcmp(dp->d_name, ".") ||
						!strcmp(dp->d_name, ".."))
					continue;
		
				strcpy(acRealUser, dp->d_name);

				Dprint("User name is %s\n", acRealUser);
				set_uid_flag = FALSE; 

				if ((f = fpOpenTable("r")) == NULL) {
					sprintf(Dbuf,"cron: cannot open table"
					   " '%s' > /dev/console", acRealUser);
					system(Dbuf);
				}
				while (tex() != EOF)
					;
				fclose(f);
			} /* while readdir */
			rewinddir(dirp);
			/* Wait for the children */
			while ((child_pid = wait(&n)) != -1) {
				static child_id	*pstDone;

				Dprint("\nmain: child id %d",child_pid);
				Dprint("\treturn is %d\n", n);
				pstDone = find_entry(child_pid);
				if (n) {
					if (ifmail(pstDone))
						mail_entry(pstDone);
					if (logflag == TRUE)
						fprintf(stderr, "User %s."
						 "Time %sCommand \"%s\".\n\n", 
						      pstDone->name, 
						      ctime(&(pstDone->time)), 
						      pstDone->command);
				}
				current = del_entry(pstDone);
				Dprint("main: current is %d\n", current->pid);
			} /* while wait */
			if (errno == ECHILD)
				pause();
			
		} /* if 320 */
	} /* for */
} /* main */

/*
 * Check do user want to have a mail messages. 
 */
int ifmail(entry)
child_id	*entry;
{
	char	*cBuf;
	int	fd;	/* Descriptor of lock file */

	if ((cBuf = malloc(sizeof(D_MAIN) + strlen(entry->name) + 1)) == NULL) {
		fprintf(stderr, "cron: out of memory\n");
		return(0);	
	}
	sprintf(cBuf, "%s/%s", D_MAIN, entry->name);
	fd = open(cBuf, 0);
	free(cBuf);
	if (fd == -1)
		return(1);
	close(fd);
	return(0);
}

/*
 * Test and Execute: tests a crontab entry against the time fields in `tm' to
 * see if it should be executed, if so it executes. f is left pointing to the
 * next entry (line). Returns EOF when encountered, something else otherwise.
 */
tex()
{
	register int fieldnum;

	for (fieldnum = MIN; fieldnum <= WDAY; ++fieldnum)
		if (!valid(fieldnum))
			return (skip_it());
	return (do_it());
}

/*
 * Valid(fieldnum) parses the next time field from the current line in crontab
 * and checks whether tmfield[fieldnum] satisfies the constraints of that time
 * field. Returns TRUE if so, FALSE if not. Leaves f at the next field.
 * Detects syntax errors in the time fields.
 */
valid(fieldnum)
int fieldnum;
{
	register int t;
	register int ival;
	register int state = START;
	int ival2;
	int tm_val = tmfield[fieldnum];

	if (fieldnum == MIN) {
		while ((t = gettoken()) == WS  ||  t == NEWLINE)
			;
		ungettoken(t);
	}

	for (;;) {
		t = gettoken();
		switch (state) {
		case START:
			switch (t) {
			case INT:
				if (strlen(tokbuf) <= 2) {
					ival = atoi(tokbuf);
					state = INT;
				}
				else
					state = ERR;
				break;
			case GLOB:
				state = GLOB;
				break;
			case WS:
				state = END;
				break;
			default:
				state = ERR;
				break;
			}
			break;
		case INT:
			if (t == COMMA  ||  t == WS) {
				if (ival == tm_val) {
					while (t != WS)
						t = gettoken();
					return (TRUE);
				}
				state = (t == WS) ? END : START;
			}
			else if (t == DASH)
				state = INTDASH;
			else
				state = ERR;
			break;
		case INTDASH:
			if (t == INT  &&  strlen(tokbuf) <= 2) {
				ival2 = atoi(tokbuf);
				state = RANGE;
			}
			else
				state = ERR;
			break;
		case RANGE:
			if (t == COMMA  ||  t == WS) {
				if (ival <= tm_val  &&  tm_val <= ival2) {
					while (t != WS)
						t = gettoken();
					return (TRUE);
				}
				state = (t == WS) ? END : START;
			}
			else
				state = ERR;
			break;
		case GLOB:
			if (t == COMMA  ||  t == WS) {
				while (t != WS)
					t = gettoken();
				return (TRUE);
			}
			else
				state = ERR;
			break;
		}		/* End switch on state */

		if (state == END)
			return (FALSE);
		if (state == ERR) {
			ungettoken(t);
			ungettoken(skip_it());
			return (FALSE);
		}
	}
}


ungettoken(t)
int t;
{
	ugtokflag = TRUE;
	ugtoken = t;
}

gettoken()
{
	register int c;
	register char *sp = tokbuf;
	register char *mark;
	int posn;

	if (ugtokflag) {
		ugtokflag = FALSE;
		return (ugtoken);
	}

	switch (c = fetch()) {
	case NEWLINE:
	case EOF:
	case PERCENT:
		return (c);
	case ' ':
	case '\t':
		while ((c = fetch()) == ' '  ||  c == '\t')
			;
		unfetch(c);
		return (WS);
	case '-':
		return (DASH);
	case ',':
		return (COMMA);
	case '*':
		return (GLOB);
	}


	/*
	 * Case of INT. Place ascii digit string into tokbuf.
	 */
	if (isdigit(c)) {
		*sp++ = c;
		while (isdigit(c = fetch()))
			*sp++ = c;
		unfetch(c);
		*sp = '\0';
		return (INT);
	}

	/*
	 * The only remaining possibility is the token STR or STRPLUS.
	 */
	*sp++ = c;
	mark = tokbuf + buflen - 2;
	while ((c = fetch()) != EOF  &&  c != NEWLINE  &&  c != PERCENT) {
		if (sp == mark) {
			posn = sp - tokbuf;
			Dprint("cron: realloc on %d bytes\n", 128);
			tokbuf = realloc(tokbuf, (buflen += 128));
			sp = tokbuf + posn;
			mark = sp + buflen;
		}
		*sp++ = c;
	}
	*sp = '\0';
	if (c == PERCENT)
		return (STRPLUS);
	unfetch(c);
	return (STR);
}


unfetch(c)
int c;
{
	ufetval = c;
	ufetflag = TRUE;
}

fetch()
{
	register int c;
	register int c2;

	if (ufetflag) {
		ufetflag = FALSE;
		return (ufetval);
	}

	for (;;)
		switch (c = getc(f)) {
		case '%':
			return (PERCENT);
		case '\n':
			return (NEWLINE);
		case '\\':
			if ((c2 = getc(f)) == '%')
				return ('%');
			else if (c2 == '\n')
				continue;
			else {
				ungetc(c2, f);
				return ('\\');
			}
		default:
			return (c);
		}
}


skip_it()
{
	register int t;

	while ((t = gettoken()) != EOF  &&  t != NEWLINE)
		;
	return (t);
}

do_it()
{
	register int c;
	register FILE *fp;

	if ((c = gettoken()) != STR  &&  c != STRPLUS) {
		ungettoken(c);
		return (skip_it());
	}
	
	Dprint("Tokken is %s\n", tokbuf);

	if ((fp = cronpipe(tokbuf, "w")) == NULL) {
		fprintf(stderr, "cron:\tCould not popen: %s\n\t\
				errno = %d\n", tokbuf, errno);
		return (skip_it());
	}

	if (c == STR) {
		fclose(fp);
		return (gettoken());
	}
	while ((c = fetch()) != NEWLINE  &&  c != EOF)
		if (c == PERCENT)
			putc('\n', fp);
		else
			putc(c, fp);
	putc('\n', fp);
	fclose(fp);
	return(c);
}

/*
 * We have to catch all signals.
 * Ignore signals INT, HUP, & PIPE.
 * Reset time on ALRM.
 * Remove lock file and take default action on the rest of them.
 */
sigsetup()
{
	register int	i;
	int 		catchalarm();	/* Catch alarm signal */
	int 		catchsignals();	/* Catch all signals */

	for (i = 1; i < NSIG; i++)
		signal(i, catchsignals);

	signal(SIGINT, SIG_IGN);
	signal(SIGHUP, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGALRM, catchalarm);
	signal(SIGCLD, SIG_DFL);
}

/*
 * Remove lock file. Restore default action. Send the catched signal to itself.
 */
int catchsignals(sig)
int	sig;
{
	unlink(F_LOCK);		/* Remove lock FIFO. Do not care if unlink */
				/* failed (case cron 3.2.0) */
	Dprint("Remove lock file %s\n", F_LOCK);
	signal(sig, SIG_DFL);	/* Reset to default */
	kill(getpid(), sig);	/* Send catched signal */
	wait(); 
}

/* 
 * Catch alarm.
 */
int catchalarm()
{
	signal(SIGALRM, catchalarm);
	time(&clock);
	tm = localtime(&clock);
	alarm(61 - tm->tm_sec);
}

