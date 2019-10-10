/* (-lgl
 * 	COHERENT Version 3.0
 * 	Copyright (c) 1982, 1993 by Mark Williams Company.
 * 	All rights reserved. May not be copied without permission.
 -lgl) */
/*
 * Structure for the /etc/group file.
 */

#ifndef	__GRP_H__
#define	__GRP_H__

struct group {
	char	*gr_name;
	char	*gr_passwd;
	int	gr_gid;
	char	**gr_mem;
};

struct	group	*getgrent();
struct	group	*getgrgid();
struct	group	*getgrnam();

#endif
