/* $Header: /src386/bin/db/RCS/i8086trace.h,v 1.1 92/06/10 14:36:50 bin Exp Locker: bin $
 *
 *	The information contained herein is a trade secret of Mark Williams
 *	Company, and  is confidential information.  It is provided  under a
 *	license agreement,  and may be  copied or disclosed  only under the
 *	terms of  that agreement.  Any  reproduction or disclosure  of this
 *	material without the express written authorization of Mark Williams
 *	Company or persuant to the license agreement is unlawful.
 *
 *	COHERENT Version 2.3.35
 *	Copyright (c) 1982, 1983, 1984.
 *	An unpublished work by Mark Williams Company, Chicago.
 *	All rights reserved.
 */
/*
 * A debugger.
 * Machine dependent header for the Intel 8086.
 *
 * $Log:	i8086trace.h,v $
 * Revision 1.1  92/06/10  14:36:50  bin
 * Initial revision
 * 
 * Revision 1.1	88/10/17  04:04:28 	src
 * Initial revision
 * 
 */

#define	I	077777			/* Integer infinity */
#define LI	017777777777L		/* Long infinity */
#define MLI	020000000000L		/* Minus long infinity */

/*
 * Formats.
 */
#define	INLEN	1			/* Size of smallest instruction */
#define	VAWID	8			/* Size of virtual address */
#define	DDCHR	'x'			/* Default debugger format */
#define DAFMT	"%04x"			/* Display address format */
#define VAFMT	"%08x"			/* Virtual address */

/*
 * Breakpoint instruction size definition.
 */
typedef char	BIN[1];
