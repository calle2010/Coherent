/*
 * /usr/include/sys/wait.h
 *
 * Support for wait() and waitpid() system calls.
 *
 * Revised Mon Mar 22 13:10:53 1993 CST
 */

#ifndef	__SYS_WAIT_H__
#define	__SYS_WAIT_H__

/*
 * This header is defined in the POSIX.1 standard ISO/IEC 9945-1:1990, and as
 * such client programs which include this header should not use any symbols
 * which end in "_t".
 */

/*
 * The contents of this header are also defined by the provisions of binary
 * compatibility standards such as the iBCS2 specification and the System V
 * ABI. The visibility of certain symbols defined by those standards may be
 * controlled by feature-test macros such as _SYSV3 for iBCS2, and _SYSV4 for
 * System V, Release 4.
 */

#include <sys/ccompat.h>
#include <sys/__pid.h>


#define	__WSIGMASK		0x7F
#define	__WSTOPFLG		0x7F

#if	_SYSV4

# define	WEXITED		0x01
# define	WTRAPPED	0x02

# define	WSTOPFLG	__WSTOPFLG
# define	WCOREFLG	0x80

# define	WCONTINUED	0x08
# define	WNOWAIT		0x80

# define	WCOREDUMP(stat)	((stat) & WCOREFLG)

#endif


#define	WUNTRACED	0x04
#define	WNOHANG		0x40


#define	__WLOBYTE(stat)		((stat) & 0xFF)
#define	__WHIBYTE(stat)		(((stat) >> 8) & 0xFF)

#define	WIFEXITED(stat)		(__WLOBYTE (stat) == 0)
#define	WEXITSTATUS(stat)	(__WHIBYTE (stat))

#define	WIFSIGNALED(stat)	(__WLOBYTE (stat) > 0 && __WHIBYTE (stat) == 0)
#define	WTERMSIG(stat)		(__WLOBYTE (stat) & __WSIGMASK)

#define	WIFSTOPPED(stat)	(__WLOBYTE (stat) == __WSTOPFLG && \
				 __WHIBYTE (stat) != 0)
#define	WSTOPSIG(stat)		(__WHIBYTE (stat))


__EXTERN_C_BEGIN__

__pid_t		wait		__PROTO ((int * _stat_loc));
__pid_t		waitpid		__PROTO ((__pid_t _pid, int * _stat_loc,
					  int _options));

__EXTERN_C_END__

#endif	/* ! defined (__SYS_WAIT_H__) */
