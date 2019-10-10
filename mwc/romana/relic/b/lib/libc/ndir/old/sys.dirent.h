/*
	COHERENT compatible
	<sys/dirent.h>	- dirent defs. for use with libndir routines by
			  D A Gwyn.
*/

#ifndef SDIRENT_H 
#define SDIRENT_H 1

#include <sys/dir.h> 
#ifdef DIR
#undef DIR
#endif

struct dirent {
	long	d_ino;	/* i-node number */
	daddr_t	d_off;	/* offset in actual directory*/
	unsigned short	d_reclen;  /*record length*/
	char	d_name[1];
};


/* The following nonportable ugliness could have been avoided by defining
   DIRENTSIZ and DIRENTBASESIZ to also have (struct dirent *) arguments. */

#define	DIRENTBASESIZ		(((struct dirent *)0)->d_name \
				- (char *)&((struct dirent *)0)->d_ino)
#define	DIRENTSIZ( namlen )	((DIRENTBASESIZ + sizeof(int) + (namlen)) \
				/ sizeof(int) * sizeof(int))
#if 0
#define	DIRENTBASESIZ	(sizeof(struct dirent))

#define	DIRENTSIZ( namlen )  ( DIRENTBASESIZ + ((((namlen) << 1) +1) >> 1))
#endif
/* DAG -- the following was moved from <dirent.h>, which was the wrong place */

#define	MAXNAMLEN	14		/* maximum filename length */

#define DIRBLKSIZ	512		/* buffer size for getdents. */

#ifndef NAME_MAX
#define	NAME_MAX	(MAXNAMLEN - 1)	/* DAG -- added for POSIX */
#endif

#endif /*Not yet called*/

