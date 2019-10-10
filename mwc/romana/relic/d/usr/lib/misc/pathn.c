/*
 * find a file on a path in the environment, or a default path
 * with an access priveledge.
 *
 * example: pathn("helpfile", "LIBPATH", ",,\lib", "r");
 *
 * Returns full path name.
 */
#include <stdio.h>
#include <path.h>

#define PATHSIZE 64

char	*getenv(), *path(), *strchr();
char *
pathn(name, envpath, deflpath, acs)
char *name, *envpath, *deflpath, *acs;
{
	register char *pathptr;

	if ((NULL == envpath) || (NULL == (pathptr = getenv(envpath))))
		pathptr = deflpath;

	if (NULL == strchr(acs, 'w')) {
		if ((pathptr = path(pathptr, name, AREAD)) == NULL)
			return (name);
		else
			return (pathptr);
	}
	else {
		register char *p, c;
		static char fullname[PATHSIZE];

		if ((p = path(pathptr, name, AWRITE)) == NULL) {
			for (p = fullname; (c = *pathptr++) && c != LISTSEP;)
				*p++ = c;
			*p++ = PATHSEP;
			strcpy(p, name);
			return (fullname);
		} else
			return (p);
	}
}
