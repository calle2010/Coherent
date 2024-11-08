/*
 * Standard I/O Library formatted input
 * Non-portable things:
 * 1) alignment of arguments is assumed to be completely contiguous.
 */

#include <stdio.h>

union	alltypes {
	char	*c;
	short	*h;
	int	*i;
	unsigned *u;
	long	*l;
	double	*d;
	char	*s;
};


/*
 * scan standard input
 */

int
scanf(args)
union alltypes args;
{
	return (xscanf(stdin, &args));
}


/*
 * scan given file
 */

int
fscanf(fp, args)
FILE *fp;
union alltypes args;
{
	return (xscanf(fp, &args));
}


/*
 * scan given string by handcrafting file structure for getc
 */

int
sscanf(sp, args)
char	*sp;
union alltypes args;
{
	FILE	file;

	_stropen(sp, strlen(sp), &file);
	return (xscanf(&file, &args));
}


static
int
xscanf(fp, argp)
FILE	*fp;
union alltypes *argp;
{
	register int	fc, gc;
	int	base, width, gotany, supprf, longf, shortf, retval = 0;
	char	*fmt;
	extern char *index();

	fmt = *(char **)argp++;
	for (;;) {
		switch (fc = *fmt++) {
		case '\0':
			break;
		case '\t':
		case '\n':
		case ' ':
			ungetc(gc=skipws(fp), fp);
			if (gc == EOF)
				break;
			else
				continue;
		default:
		matchin:
			if ((gc=getc(fp)) != fc) {
				ungetc(gc, fp);
				break;
			} else
				continue;
		case '%':
			supprf = (fc = *fmt++) == '*';
			if (supprf)
				fc = *fmt++;
			for (width = 0; '0'<=fc && fc<='9'; fc = *fmt++)
				width = width*10 + fc - '0';
			longf = fc=='l';
			shortf = fc=='h';
			if (longf || shortf)
				fc = *fmt++;
			switch (fc) {
			default:
				fputs("Bad format in scanf\n", stderr);
				abort();
			case '\0':
				break;
			case '%':
				goto matchin;
			case 'D':
				longf++;
			case 'd':
				base = 10;
				goto fixed;
			case 'O':
				longf++;
			case 'o':
				base = 8;
				goto fixed;
			case 'N':
				longf++;
			case 'n':
				base = 0;
				goto fixed;
			case 'X':
				longf++;
			case 'x':
				base = 16;
			fixed:
				{
					long	longn, fgetnum();

					longn = fgetnum(fp, base, width, &gotany);
					if (!gotany)
						break;
					if (supprf)
						continue;
					if (longf)
						**(long **)argp++ = longn;
					else if (shortf)
						**(short **)argp++ = (short)longn;
					else
						**(int **)argp++ = (int)longn;
					retval++;
					continue;
				}
			case 'E':
			case 'F':
				longf++;
			case 'e':
			case 'f':
				{
					double	doublen, fgetdbl();

					doublen = fgetdbl(fp, width, &gotany);
					if (!gotany)
						break;
					if (supprf)
						continue;
					if (longf)
						**(double **)argp++ = doublen;
					else
						**(float **)argp++ = doublen;
					retval++;
					continue;
				}
			case '[':
				{
					char	ss[64], *s = ss;
					int	breakf;
					register char	*cp;

					breakf = (fc = *fmt++) == '^';
					if (breakf)
						fc = *fmt++;
					while (fc!='\0' && fc!=']') {
						*s++ = fc;
						fc = *fmt++;
					}
					*s = '\0';
					s = ss;
					gc = getc(fp);
					goto scanin;
			case 'c':
					if (width==0)
						width = 1;
					gc = getc(fp);
					s = "";
					breakf = 1;
					goto scanin;
			case 's':
					gc = skipws(fp);
					s = "\t\n ";
					breakf = 1;
				scanin:
					if (!supprf)
						cp = *(char **)argp++;
					gotany = 0;
					while (gc!=EOF
					    && (index(s, gc)==NULL)==breakf) {
						gotany++;
						if (!supprf)
							*cp++ = gc;
						if (--width==0)
							gc = EOF;
						else
							gc = getc(fp);
					}
					if (gc!=EOF)
						ungetc(gc, fp);
					if (!gotany)
						break;
					if (supprf)
						continue;
					if (fc!='c')
						*cp = '\0';
					retval++;
					continue;
				}
			case 'r':
				retval += xscanf(fp, *(union alltypes **)argp++);
				continue;
			}
			break;
		}
		break;
	}
	return (retval==0&&feof(fp) ? EOF : retval);
}

/*
 * Read a long integer from file fp
 */
static
long
fgetnum(fp, base, width, got)
FILE	*fp;
register int	base, width;
int	*got;
{
	long	val = 0;
	register int	c, i;
	int	neg;

	if (width <= 0 || 32 < width)
		width = 32;
	*got = 0;
	c = skipws(fp);
	if ((neg=c=='-') || c=='+')
		c = getc(fp), ++*got;
	if (*got>width)
		;
	else if (base!=0)
		;
	else if (c!='0')
		base = 10;
	else if (c=getc(fp), ++*got>width)
		;
	else if (c!='x')
		base = 8;
	else {
		base = 16;
		c = getc(fp), ++*got;
	}
	for (; *got < width; ++*got) {
		if ((10<=(i=c-('a'-10))
		 || 10<=(i=c-('A'-10))
		 || 0<=(i=c-'0') && i<=9)
		 && i<base
		 || base==16 && val==0L && (i=c-'x')==0) {
			val = val*base - i;
			c = getc(fp);
		} else
			break;
	}
	ungetc(c, fp);
	return (neg ? val : -val);
}

/*
 * Scan off floating point number into buffer for atof
 * using automaton shown below
 *	states in parantheses
 *	inputs in brackets
 *	for all inputs not shown, transition is to (end)
 *
 *                                 --
 *                               /    \
 *                              v      \
 *                ----------> [0-9] -> (3)
 *              /               ^       | \
 *             /               /        |  \
 *            /               /         v   \
 *           /               /         [.]   \               ->[+-] -
 *          /               /           |     \             /         \
 *         /               /            |      \           /           \
 *        /               /             v       v         /             v
 *     (0) --> [+-] -> (1)             (4) --> [Ee] -> (6) -> [0-9] -> (7)
 *        \           /               /         ^               ^      /
 *         \         /               /         /                 \    /
 *          \       /               /         /                    --
 *           \     /               /         /
 *            \   /               /         /
 *             \ /               /         /
 *              v               v         /
 *             [.] --> (2) -> [0-9] -> (5)
 *                              ^      /
 *                               \    /
 *                                 --
 *
 */
static
double
fgetdbl(fp, width, got)
FILE	*fp;
int	width, *got;
{
	double atof();
	register int	c, state = 0;
	char	str[33];
	register char	*s = str;

	if (width<=0 || 32<width)
		width = 32;
	for (*got = 0; *got < width; ++*got) {
		switch (c=getc(fp)) {
		case '\t':
		case '\n':
		case ' ':
			if (state!=0)
				break;
			--*got;
			continue;
		case '+':
		case '-':
			if (state!=0 && state!=6)
				break;
			state++;
			*s++ = c;
			continue;
		case '.':
			if (state<=1)
				state = 2;
			else if (state==3)
				state++;
			else
				break;
			*s++ = c;
			continue;
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
			if (state==0 || state==1 || state==3)
				state = 3;
			else if (state==2 || state==4 || state==5)
				state = 5;
			else if (state==6 || state==7)
				state = 7;
			else
				break;
			*s++ = c;
			continue;
		case 'E':
		case 'e':
			if (state<3 || 5<state)
				break;
			state = 6;
			*s++ = c;
			continue;
		default:
			break;
		}
		ungetc(c, fp);
		break;
	}
	*s = '\0';
	return (atof(str));
}

static
int
skipws(fp)
FILE	*fp;
{
	register int	c;

	while ((c=getc(fp))==' ' || c=='\n' || c=='\t')
		;
	return (c);
}
