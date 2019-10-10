/*
 * An editor.
 * Commands.
 */
#include <stdio.h>
#include <ctype.h>
#include "ed.h"

/*
 * Edit a file.  The filename is stored in `file'.
 */
edit()
{
	register int ret, i;

	if (doladd > 0)
		delete(1, doladd);
	tmpseek = CLSIZE;
	rcurbno = -1;
	wcurbno = -1;
	if ((fp=xfopen(file, "r")) == NULL) {
		derror("Cannot open file");
		return (0);
	}
#if RSX
	if ((i=fp->v_efbk+1-tmp->v_hibk) > 0) {
		if (grow(i) == 0) {
			terror("Temp file error");
			return (0);
		}
	}
#endif
	ret = 1;
	if (append(0, readfil) == 0)
		ret = 0;
	else if (ferror(fp)) {
		derror("Read error");
		ret = 0;
	}
	fclose(fp);
	for (i=0; i<MKSIZE; i++)
		marklin[i] = 0;
	saved = 1;
	if (cflag != 0) {
		printl(stdout, !oflag ? cct : lct);
		printc(stdout, '\n');
	}
	return (ret);
}

/*
 * Make sure that the previous command is terminated either
 * immediately by a newline or has a 'p' or a 'l' which is
 * then terminated by a newline.  The variable `vcom' is set
 * to the verify command.  A zero is returned if there were
 * bad characters before the newline, otherwise a one.
 */
verify(flag)
{
	register int c;

	vcom = ' ';
	if (flag==0 && mflag!=0)
		return (1);
	if ((c=getx()) == '\n') {
		ungetx(c);
		return (1);
	}
	if (c=='l' || c=='p') {
		if (getx() == '\n') {
			ungetx('\n');
			vcom = c;
			return (1);
		}
	}
	derror("Bad command");
	return (0);
}

/*
 * Get a filename of the form required for the r command.
 * which is returned in `name'.  If no name is specified,
 * the one stored in `file' is copied.  If a file name is
 * specified and none exists in `file', it is copied to
 * `file'.  If an illegal name is given, an error is derrord.
 */
getfile(name)
char name[FNSIZE+1];
{
	register int c;
	register char *p1, *p2;

	switch (c=getx()) {
	case '\n':
		p1 = name;
		p2 = file;
		while (*p1++ = *p2++)
			;
		break;
	case ' ':
	case '\t':
		p1 = name;
		p2 = &name[FNSIZE];
		while ((c=getx()) == ' ' || c == '\t')
			;
		*p1++ = c;
		while ((c=getx()) != EOF && c != '\n') {
			if (p1 >= p2) {
				derror("File name too long");
				name[0] = '\0';
				return (0);
			}
			*p1++ = c;
		}
		*p1 = '\0';
		if (c == EOF) {
			derror("EOF in file name");
			name[0] = '\0';
			return (0);
		}
		if (file[0] == '\0') {
			p1 = file;
			p2 = name;
			while (*p1++ = *p2++)
				;
		}
		break;
	default:
		while (getx() != '\n')
			;
		derror("Bad command");
		return (0);
	}
	if (name[0] == '\0') {
		derror("Null file name");
		return (0);
	}
	return (1);
}

/*
 * Read the rest of the input line into `tempbuf'.
 */
rest()
{
	register int c;
	register char *cp;

	cp = tempbuf;
	while ((c=getx())!=EOF && c!='\n') {
		if (cp >= &tempbuf[TBSIZE-1]) {
			derror("Line too long");
			return (0);
		}
		*cp++ = c;
	}
	*cp = '\0';
	return (1);
}

/*
 * Append text after the given address.  A line of text is gotten
 * by calling the given function until it returns NULL.
 */
append(a, f)
int (*f)();
{
	int seek;
	register int n;

	cct = 0;
	lct = 0;
	if (a<0 || a>doladd) {
		derror("Address out of range");
		return (0);
	}
	dotadd = a;
	while ((n=(*f)()) != 0) {
		if (intflag)
			return (1);
		seek = linead();
		if (putline(linebuf, n) == 0)
			return (0);
		saved = 0;
		cct += n;
		lct++;
		if (expand(dotadd) == 0)
			return (0);
		line[++dotadd] = seek;
	}
	return (1);
}

/*
 * Expand the line table and leave a hole at the given address.
 */
expand(a)
{
	register LINE *lp1;
	register LINE *lp2;
	register int n;

	if (doladd+3 >= lnsize) {
		lp1 = (LINE *)realloc(line, (n=lnsize*2)*sizeof(LINE));
		if (lp1 == NULL) {
			derror("Line table overflow");
			return (0);
		}
		line = lp1;
		lnsize = n;
	}
	lp1 = &line[doladd+2];
	lp2 = &line[doladd+1];
	n = doladd++ - a;
	while (n--)
		*--lp1 = *--lp2;
	return (1);
}

/*
 * Get a line of text from the terminal.  The number of characters
 * including the null terminator at the end of the string is returned.
 * If end of file or a line containing only a single dot is found,
 * 0 is returned.
 */
readtty()
{
	register int c;
	register char *lp;

	appflag++;
	lp = linebuf;
	while ((c=getx())!=EOF && c!='\n') {
		if (lp < &linebuf[LBSIZE-1])
			*lp++ = c;
	}
	--appflag;
	if (c==EOF && lp==linebuf) {
		lastchr = '\n';
		if (gcp == NULL)
			clearerr(stdin);
		return (0);
	}
	if (linebuf[0]=='.' && lp==linebuf+1)
		return (0);
	*lp++ = '\0';
	return (lp-linebuf);
}

/*
 * Get a line of text from the file open on file pointer `fp'.
 * Return the number of characters in the line including the
 * null terminator at the end.  On end of file, 0 is returned.
 */
readfil()
{
	register int c, n;
	register char *lp;

	if ((c=getc(fp)) == EOF)
		return (0);
	lp = linebuf;
	n = LBSIZE-1;
	while (c!=EOF && c!='\n') {
		*lp++ = c;
		if (--n == 0)
			break;
		c = getc(fp);
	}
	*lp++ = '\0';
	return (lp-linebuf);
}

/*
 * Delete the given line range.
 */
delete(a1, a2)
{
	register LINE *p1, *p2;
	register int n;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	p1 = &line[a1];
	p2 = &line[a2+1];
	n = doladd - a2;
	while (n--)
		*p1++ = *p2++;
	doladd -= (a2+1) - a1;
	if ((dotadd=a1) > doladd)
		--dotadd;
	saved = doladd==0;
	return (1);
}

/*
 * Concatenate the lines in the given range to form
 * a single line.
 */
join(a1, a2)
{
	long seek;
	int bn, a;
	register int n;
	register char *lp, *tp;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	bn = 0;
	lp = linebuf;
	for (a=a1; a<=a2; a++) {
		if ((n=getline(a, tempbuf)) == 0)
			return (0);
		if ((bn+=--n) >= LBSIZE-1) {
			derror("Temporary buffer overflow");
			return (0);
		}
		tp = tempbuf;
		while (n--)
			*lp++ = *tp++;
	}
	*lp++ = '\0';
	seek = linead();
	if (putline(linebuf, ++bn) == 0)
		return (0);
	line[a1] = seek;
	a = doladd - a2;
	for (n=0; n<a; n++)
		line[a1+1+n] = line[a2+1+n];
	dotadd = a1;
	doladd -= a2 - a1;
	return (1);
}

/*
 * List the given line range.  All non-printing characters are
 * escaped.
 */
list(a1, a2)
{
	int a;
	register int n;
	register char *p;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	for (a=a1; a<=a2; a++) {
		if (intflag)
			return (1);
		if (getline(a, linebuf) == 0)
			break;
		n = 0;
		for (p=linebuf; *p; p++) {
			if (n++ >= 72) {
				n = 0;
				prints(stdout, "\\\n");
			}
			switch (*p) {
			case '\b':
				prints(stdout, "-\b<");
				continue;
			case '\t':
				prints(stdout, "-\b>");
				continue;
			case '\\':
				prints(stdout, "\\\\");
				continue;
			default:
				if (isascii(*p) && !iscntrl(*p)) {
					printc(stdout, *p);
					continue;
				}
				printc(stdout, '\\');
				printo(stdout, *p);
				n += 3;
			}
		}
		prints(stdout, "\\n");
		printc(stdout, '\n');
		dotadd = a;
	}
	return (1);
}

/*
 * Take the text that is between lines `a1' and `a2' and
 * place it after line `a3'.
 */
move(a1, a2, a3)
int a1, a2, a3;
{
	LINE l;
	register int a, n, x;

	if (a1>a2 || a1<1 || a2>doladd || a3<0 || a3>doladd) {
		derror("Address out of range");
		return (0);
	}
	if (a3>=a1-1 && a3<=a2) {
		dotadd = a2;
		return (1);
	}
	if (a3 < a1) {
		for (a=a1, x=a3+1; a<=a2; a++, x++) {
			l = line[a];
			for (n=a; n>x; --n)
				line[n] = line[n-1];
			line[x] = l;
		}
		dotadd = a3 + a2+1 - a1;
	} else {
		for (a=a2, x=a3; a>=a1; --a, --x) {
			l = line[a];
			for (n=a; n<x; n++)
				line[n] = line[n+1];
			line[x] = l;
		}
		dotadd = a3;
	}
	saved = 0;
	return (1);
}

/*
 * Given a string describing a set of options, set them.
 */
setoptf(sp)
register char *sp;
{
	register int t;

	t = 1;
	while (*sp != '\0') {
		switch (*sp++) {
		case '+':
			t = 1;
			continue;
		case '-':
			t = 0;
			continue;
		case 'c':
			cflag = t;
			continue;
		case 'm':
			mflag = t;
			continue;
		case 'o':
			oflag = t;
			continue;
		case 'p':
			pflag = t;
			continue;
		case 's':
			sflag = t;
			continue;
		case 'v':
			vflag = t;
			continue;
		default:
			derror("Bad option");
			return;
		}
	}
}

/*
 * Display options.
 */
disoptf()
{
	if (cflag)
		printc(stdout, 'c');
	if (mflag)
		printc(stdout, 'm');
	if (oflag)
		printc(stdout, 'o');
	if (pflag)
		printc(stdout, 'p');
	if (sflag)
		printc(stdout, 's');
	if (vflag)
		printc(stdout, 'v');
	if (keyp != NULL)
		printc(stdout, 'x');
	printc(stdout, '\n');
}

/*
 * Print the given line range.
 */
print(a1, a2)
{
	register int a;
	register char *p;

	if (a1>a2 || a1<1 || a2>doladd) {
		derror("Address out of range");
		return (0);
	}
	for (a=a1; a<=a2; a++) {
		if (intflag)
			return (1);
		if (getline(a, linebuf) == 0)
			break;
		p = linebuf;
		while (*p)
			printc(stdout, *p++);
		printc(stdout, '\n');
		dotadd = a;
	}
	return (1);
}

/*
 * Make a copy of the text between lines `a1' and `a2'
 * and place them after `a3'.
 */
copy(a1, a2, a3)
{
	register int i, j, n;

	if (a1>a2 || a1<1 || a2>doladd || a3<0 || a3>doladd) {
		derror("Address out of range");
		return (0);
	}
	n = (a2+1) - a1;
	if (doladd+n+2 > lnsize) {
		derror("Line table overflow");
		return (0);
	}
	for (i=doladd; i>a3; --i)
		line[i+n] = line[i];
	for (i=0; i<n; i++) {
		if ((j=i+a1) > a3)
			j += n;
		if ((j=getline(j, linebuf)) == 0)
			goto err;
		line[a3+1+i] = linead();
		if (putline(linebuf, j) == 0)
			goto err;
	}
	dotadd = a3+n;
	doladd += n;
	saved = 0;
	return (1);

err:
	for (i=a3+1; i<=doladd; i++)
		line[i] = line[i+n];
	return (0);
}

/*
 * Write the given line range onto the file whose name is
 * stored in `file'.  `perm' is the permission string we
 * want the file opened with.
 */
wfile(a1, a2, name, perm, sflag)
char *name, *perm;
{
	register int a;
	register char *cp;

	if (doladd!=0 || addspec!=0) {
		if (a1>a2 || a1<1 || a2>doladd) {
			derror("Address out of range");
			return (0);
		}
	}
	if ((fp=xfopen(name, perm)) == NULL) {
		derror("Cannot open file");
		return (0);
	}
	cct = 0;
	lct = 0;
	for (a=a1; a<=a2; a++) {
		if (getline(a, linebuf) == 0)
			break;
		cp = linebuf;
		while (*cp) {
			printc(fp, *cp++);
			cct++;
		}
		printc(fp, '\n');
		if (ferror(fp))
			break;
		cct++;
		lct++;
	}
	if (sflag==0 && cflag!=0) {
		printl(stdout, !oflag ? cct : lct);
		printc(stdout, '\n');
	}
	if ((a=ferror(fp)) == 0)
		saved = 1;
	else {
		if (sflag == 0)
			derror("Write error");
	}
	fclose(fp);
#if COHERENT
	sync();
#endif
	return (!a);
}

/*
 * Open a file.  If we are in encryption mode, the open unit is really a
 * file descriptor piped to the crypt command.
 */
FILE *
xfopen(fn, mode)
char *fn;
char *mode;
{
#if COHERENT
	register FILE *fp;
	register int f;
	register int u;
	register int a;
	register int b;
	int pv[2];

	if (keyp == NULL)
		fp = fopen(fn, mode);
	else {
		switch (mode[0]) {
		case 'a':
			return (NULL);
		case 'r':
			a = 0;
			b = 1;
			break;
		case 'w':
			a = 1;
			b = 0;
			break;
		}
		if (pipe(pv) < 0)
			return (NULL);
		if ((f=fork()) < 0) {
			close(pv[0]);
			close(pv[1]);
			return (NULL);
		}
		if (f == 0) {
			if ((u=(a==0)?open(fn, 0):creat(fn, 0644)) < 0)
				exit(1);
			dup2(u, a);
			dup2(pv[b], b);
			close(u);
			close(pv[0]);
			close(pv[1]);
			execl("/bin/crypt", "crypt", keyp, NULL);
			exit(1);
		}
		close(pv[b]);
		fp = fdopen(pv[a], mode);
	}
	return (fp);
#else
	return(fopen(fn, mode));
#endif
}

/*
 * Read a key from the standard input.
 */
setkey()
{
#if COHERENT
	register int n;

	keyp = getpass("key? ");
	if ((n=strlen(keyp)) > CKSIZE) {
		derror("Key too long");
		keyp = NULL;
		return (0);
	}
	if (n == 0)
		keyp = NULL;
	return (1);
#else
	derror("Cannot set key");
	return (0);
#endif
}
