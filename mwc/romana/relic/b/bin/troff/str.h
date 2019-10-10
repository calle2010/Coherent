/*
 * str.h
 * Nroff/Troff.
 * Input stack header.
 */

#define SREQS	0			/* Request			*/
#define SSINP	1			/* Insertion from standard input */
#define SFILE	2			/* File I/O			*/
#define SCORE	3			/* Core I/O			*/
#define SCTEX	4			/* Chained text			*/
#define SCDIV	5			/* Chained diversion		*/

#define ARGSIZE	10			/* Maximum number of arguments	*/

/*
 * Used for stacking input sources.
 */
typedef union STR {
	struct	x1 {			/* Used for insertions from stdin */
		union STR *s_next;	/* Pointer to next in stack	*/
		int	s_type;		/* Type of input		*/
		int	s_eoff;		/* End of file flag		*/
		int	s_clnc;		/* Current line number		*/
		int	s_nlnc;		/* Last line number		*/
		int	s_argc;		/* Number of arguments		*/
		char	*s_argp[ARGSIZE];	/* Pointer to arguments	*/
		char	*s_abuf;	/* Arg buffer space		*/
	} x1;
	struct	x2 {			/* File I/O			*/
		union STR *s_next;	/* Pointer to next in stack	*/
		int	s_type;		/* Type of input		*/
		int	s_eoff;		/* End of file flag		*/
		int	s_clnc;		/* Current line number		*/
		int	s_nlnc;		/* Last line number		*/
		int	s_argc;		/* Number of arguments		*/
		char	*s_argp[ARGSIZE];	/* Pointer to arguments	*/
		char	*s_abuf;	/* Arg buffer space		*/
		char	*s_fname;	/* File name			*/
		FILE	*s_fp;		/* File pointer			*/
	} x2;
	struct	x3 {			/* Core I/O			*/
		union STR *s_next;	/* Pointer to next in stack	*/
		int	s_type;		/* Type of input		*/
		int	s_eoff;		/* End of file flag		*/
		int	s_clnc;		/* Current line number		*/
		int	s_nlnc;		/* Last line number		*/
		int	s_argc;		/* Number of arguments		*/
		char	*s_argp[ARGSIZE];	/* Pointer to arguments	*/
		char	*s_abuf;	/* Arg buffer space		*/
		char	*s_cp;		/* Pointer in core		*/
		char	*s_srel;	/* Released when unstacked	*/
	} x3;
	struct	x4 {			/* Chained I/O			*/
		union STR *s_next;	/* Pointer to next in stack	*/
		int	s_type;		/* Type of input		*/
		int	s_eoff;		/* End of file flag		*/
		int	s_clnc;		/* Current line number		*/
		int	s_nlnc;		/* Last line number		*/
		int	s_argc;		/* Number of arguments		*/
		char	*s_argp[ARGSIZE];	/* Pointer to arguments	*/
		char	*s_abuf;	/* Arg buffer space		*/
		char	*s_bufp;	/* Pointer to buffer		*/
		char	*s_bufend;	/* End of buffer in core	*/
		union MAC *s_macp;	/* Next in chain list		*/
		int	s_disk;		/* Data is on disk		*/
		char	*s_sp;		/* Position in core		*/
		unsigned long s_seek;	/* Seek position		*/
		int	s_n;		/* Counter (count down)		*/
	} x4;
} STR;

/*
 * Global variables.
 */
extern	STR	*strp;			/* Input stack */

/* end of str.h */
