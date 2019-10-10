/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	 __SYS_MTIOCTL_H__
#define	 __SYS_MTIOCTL_H__

/*
 * Magnetic tape ioctl commands.
 */

#define MTIOC		('M'<<8)
#define MTREWIND 	(MTIOC|1)	/* Rewind */
#define	MTWEOF	 	(MTIOC|2)	/* Write end of file mark */
#define MTRSKIP	 	(MTIOC|3)	/* Record skip */
#define MTFSKIP	 	(MTIOC|4)	/* File skip */
#define MTDEC	 	(MTIOC|5)	/* DEC mode */
#define MTIBM	 	(MTIOC|6)	/* IBM mode */
#define MT800	 	(MTIOC|7)	/* 800 bpi */
#define MT1600	 	(MTIOC|8)	/* 1600 bpi */
#define	MT6250	 	(MTIOC|9)	/* 6250 bpi */

#define MTTENSE	 	(MTIOC|10)	/* Tension tape */
#define	MTERASE	 	(MTIOC|11)	/* Erase tape */

#endif	/* ! defined (__SYS_MTIOCTL_H__) */
