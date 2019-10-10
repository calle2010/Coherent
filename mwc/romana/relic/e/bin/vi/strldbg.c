/* strldgb.c - debugging version of strlen */

#include <fcntl.h>

int fddb = -1;

/*
 * stack:
 *	Arg
 *	Return Addr
 */
int strlen(cp)
char * cp;
{
	int * dbp;
	char buf[80];

	if (fddb == -1) {
		fddb = open("/dev/console", O_RDWR);
	}

	dbp = (int *)&cp;
	dbp--;
	sprintf(buf, "from %08x: strlen(%08x)\n", *dbp, cp);
	write(fddb, buf, stl(buf));
	return stl(cp);
}

int stl(cp)
char * cp;
{
	int ret = 0;
	while (*cp++)
		ret++;
	return ret;
}
