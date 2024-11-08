/*
 * Prepare text for statistical processing
 * by breaking it into words (and possibly
 * also punctuation marks) and discarding
 * certain words if this is desired.
 */

#include <stdio.h>
#include <ctype.h>

#define	NHASH	64		/* Hash buckets for ignore and only */
#define	NWORD	400		/* Longest word */

typedef	struct	WORDS {
	struct	WORDS	*w_next;
	char	w_name[];
}	WORDS;

WORDS	*words[NHASH];

int	pflag;			/* Print punctuation as well */
int	dflag;			/* Print (input) word numbers */
int	fflag;			/* Fold upper into lower case */
int	iflag;			/* Ignore case for '-i' */
int	nignore;		/* Number of ignored words */
int	nonly;			/* Number of only words */
long	wordno;			/* Input word number */

char	wordbuf[NWORD];

char	missing[] = "Missing `%s' file argument";
char	onlyone[] = "Only one of `-i' or `-o' may be given";

main(argc, argv)
int argc;
char *argv[];
{
	register char *ap;
	register int i;
	register int estat = 0;
	register FILE *fp;

	while (argc>1 && *argv[1]=='-') {
		for (ap = &argv[1][1]; *ap != '\0'; ap++)
			switch (*ap) {
			case 'd':
				dflag = 1;
				break;

			case 'f':
				fflag = 1;
				break;

			case 'p':
				pflag = 1;
				break;

			case 'i':
				if (nonly)
					preperr(onlyone);
				if (argc < 3)
					preperr(missing, "ignore");
				argv++;
				argc--;
				nignore += enter(argv[1]);
				iflag = 1;
				break;

			case 'o':
				if (nignore)
					preperr(onlyone);
				if (argc < 3)
					preperr(missing, "only");
				argv++;
				argc--;
				nonly += enter(argv[1]);
				break;

			default:
				usage();
			}
		argv++;
		argc--;
	}
	if (argc > 1)
		for (i=1; i<argc; i++) {
			if ((fp = fopen(argv[i], "r")) == NULL)
				preperr("Cannot open `%s'", argv[i]);
			estat |= prep(fp);
			fclose(fp);
		}
	else
		estat = prep(stdin);
	exit(estat);
}

/*
 * Run prep on each input file.
 */
prep(fp)
FILE *fp;
{
	register char *cp;
	register int c;
	register int inword = 0;

	while ((c = getc(fp)) != EOF) {
		if (!isascii(c))
			c = '\0';
		if (fflag && isupper(c))
			c = tolower(c);
		if (inword) {
			if (isalpha(c) || c=='\'') {
				*cp++ = c;
				continue;
			}
			if (c == '-') {
				if ((c = getc(fp)) == '\n')
					continue;
				ungetc(c, fp);
				c = '-';
			}
			*cp = '\0';
			inword = 0;
			wordno++;
			print(wordbuf);
		}
		if (isalpha(c) || c=='\'') {
			inword++;
			cp = wordbuf;
			*cp++ = c;
		} else if (pflag && ispunct(c)) {
			putchar(c);
			putchar('\n');
		}
	}
}

/*
 * Print out a word.
 */
print(word)
char *word;
{
	if ((nignore && lookup(word)) || (nonly && !lookup(word)))
		return;
	if (dflag)
		printf("%ld\t", wordno);
	printf("%s\n", wordbuf);
}

/*
 * Enter words from the given file
 * into the hash table.
 */
enter(fn)
char *fn;
{
	register char *cp;
	register WORDS *wp;
	register int c;
	register unsigned hash;
	register int nword = 0;
	register FILE *fp;

	if ((fp = fopen(fn, "r")) == NULL)
		preperr("Cannot open `%s'", fn);
	while (fgets(wordbuf, NWORD, fp) != NULL) {
		hash = 0;
		cp = wordbuf;
		while ((c = *cp++) != '\0') {
			if (c == '\n') {
				cp[-1] = '\0';
				break;
			}
			if (isupper(c))
				cp[-1] = c = tolower(c);
			hash += c;
		}
		if ((wp = (WORDS *)malloc(sizeof(WORDS) + cp-wordbuf)) == NULL)
			preperr("Out of memory for words from `%s'", fn);
		strcpy(wp->w_name, wordbuf);
		wp->w_next = words[hash %= NHASH];
		words[hash] = wp;
		nword++;
	}
	fclose(fp);
	return (nword);
}

/*
 * Lookup a word in either the only
 * or exception list.
 */
lookup(word)
char *word;
{
	register WORDS *wp;
	register char *cp;
	register unsigned hash = 0;
	char	wordbuf[256];		/* Keep a temporary copy of a world */
	int	i;

	cp = word;
	while (*cp != '\0')
		if (isupper(*cp))
			hash += tolower(*cp++); else
			hash += *cp++;
	for (wp = words[hash%NHASH]; wp != NULL; wp = wp->w_next) {
		if (iflag && !fflag) {
			if (strlen(word) > 256)
				preperr("word too long %s", word);
			for (cp = word, i = 0; *cp != '\0'; cp++)
				if (isupper(*cp))
					wordbuf[i++] = tolower(*cp);
				else 
					wordbuf[i++] = *cp;
			wordbuf[i] = '\0';
			cp = wordbuf;
			if (strcmp(wp->w_name, cp) == 0) 
				return (1);
		} else {	
			if (strcmp(wp->w_name, word) == 0) 
				return (1);
		}
	}
	return (0);
}

/* VARARGS */
preperr(x)
{
	fprintf(stderr, "prep: %r\n", &x);
	exit(1);
}

usage()
{
	fprintf(stderr, "Usage: prep [-dfp] [-i file] [-o file] [file ...]\n");
	exit(1);
}
