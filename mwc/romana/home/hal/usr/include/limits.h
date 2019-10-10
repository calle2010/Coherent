/* (-lgl
 *	Coherent 386 release 4.2
 *	Copyright (c) 1982, 1993 by Mark Williams Company.
 *	All rights reserved. May not be copied without permission.
 *	For copying permission and licensing info, write licensing@mwc.com
 -lgl) */

#ifndef	__LIMITS_H__
#define	__LIMITS_H__

#include <common/feature.h>
#include <common/_limits.h>

#define	CHAR_BIT		__CHAR_BIT
#define	UCHAR_MAX		__UCHAR_MAX
#define	CHAR_MAX		__CHAR_MAX
#define	CHAR_MIN		__CHAR_MIN
#define	SCHAR_MAX		__SCHAR_MAX
#define	SCHAR_MIN		__SCHAR_MIN

#define	USHRT_MAX		__USHRT_MAX
#define	SHRT_MAX		__SHRT_MAX
#define	SHRT_MIN		__SHRT_MIN

#define	UINT_MAX		__UINT_MAX
#define	INT_MAX			__INT_MAX
#define	INT_MIN			__INT_MIN

#define	ULONG_MAX		__ULONG_MAX
#define	LONG_MAX		__LONG_MAX
#define	LONG_MIN		__LONG_MIN

#define	MB_LEN_MAX		1

/*
 * This limit applies to the Coherent libc.
 */

#define	TZNAME_MAX		31

#if	! _STDC_SOURCE && ! _SYSV4

/*
 * System V, Release 3 systems have the following limits hard-coded; the
 * actual limits are from iBCS2 except for NAME_MAX, which is supposed to
 * be flexible. However, a NAME_MAX of PATH_MAX seems a reasonable
 * compromise for now (SCO systems actually have many of the constants below
 * flexibly defined rather than set for iBCS2 anyway...)
 */

#define	ARG_MAX			5120
#define	LINK_MAX		1000
#define	NAME_MAX		256
#define	PATH_MAX		256
#define	PIPE_BUF		5120
#define	PIPE_MAX		5120
#define	PASS_MAX		8
#define	CHILD_MAX		25
#define	NGROUPS_MAX		32

#endif

#if	! _POSIX_C_SOURCE
/*
 * System V, Release 4 systems provide the following additional limits. The
 * limits defined here are not in the ABI, but the values here (as taken from
 * the System V, Release 4 Programmer's Reference Manual) are correct for
 * iBCS2 systems.
 */

#define	PID_MAX			30000
#define	UID_MAX			60000
#define	SYSPID_MAX		1

#endif

#endif	/* ! defined (__LIMITS_H__) */
