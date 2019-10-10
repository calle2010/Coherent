/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__COMMON__STREAM_H__
#define	__COMMON__STREAM_H__

/*
 * This internal header file declares the STREAMS data types exported to
 * drivers via the header <sys/stream.h>.  Several high-profile kernel data
 * structures need to declare pointers to these items, so we export the
 * following incomplete type declarations.
 */

typedef struct msgb	mblk_t;
typedef struct datab	dblk_t;
typedef struct qband	qband_t;
typedef struct queue	queue_t;


/*
 * The following type does not belong to the DDI/DKI STREAMS definition; it is
 * used internally.
 */

typedef struct __streams_schedule ssched_t;

#endif	/* ! defined (__COMMON__STREAM_H__) */
