/*
 * codebug.h
 * Nroff/Troff.
 * Debugging header.
 */

#ifndef	CODEBUG_H
#define	CODEBUG_H		/* Process this only once...	*/

#ifndef	DDEBUG
#define	DDEBUG	0		/* Default to a level of 0	*/
#endif

#define	DBGCHEK	0x0001		/* Various check points	*/
#define	DBGREGS	0x0002		/* Register creation	*/
#define	DBGREGX	0x0004		/* Register access	*/
#define	DBGCODE	0x0008		/* Output codes		*/
#define	DBGDIVR	0x0010		/* Diversions and traps	*/
#define	DBGFILE	0x0020		/* File access		*/
#define	DBGFUNC	0x0040		/* Various functions	*/
#define	DBGCHAR 0x0080		/* Characters		*/
#define	DBGPROC	0x0100		/* "process" debugging	*/
#define	DBGMACX	0x0200		/* Macro execution	*/
#define	DBGREQX 0x0400		/* request processing	*/
#define	DBGMISC	0x0800		/* Misc. things		*/
#define	DBGMOVE	0x1000		/* "pel" movement	*/
#define	DBGENVR	0x2000		/* Environment		*/
#define	DBGCALL	0x4000		/* Special call stuff	*/

#if	DDEBUG
#define	dprintd(a,b)	printd(a, b)		/* debug information	*/
#define	dprint2(a,b,c)	printd(a, b, c)		/* debug info with arg.	*/
#define	dprint3(a,b,c,d) printd(a, b, c, d)	/* debug info with args	*/
#else
#define	dprintd(a, b)				/* Not without debug!	*/
#define	dprint2(a, b, c)			/*			*/
#define	dprint3(a, b, c, d)			/*			*/
#endif

extern int dbglvl;				/* Debugging level.	*/
#endif

/* end of codebug.h */
