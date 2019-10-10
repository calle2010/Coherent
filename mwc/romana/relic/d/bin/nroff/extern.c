/*
 * extern.c
 * Nroff/Troff.
 * Global variables.
 */

#include "roff.h"

/* Miscellaneous globals, declared in roff.h. */
int		A_reg;			/* .A register			*/
int		a_reg;			/* .a register			*/
char		*argv0;			/* "nroff" or "troff"		*/
int		bracelevel;		/* Level of \{ \} nesting	*/
int		byeflag;		/* True when exiting		*/
int		dbglvl;			/* Debug level			*/
int		dflag;			/* Debug flag			*/
char		diskbuf[DBFSIZE];	/* Disk buffer for temp file	*/
char		endtrap[2];		/* End macro name		*/
char		esc;			/* Escape character		*/
int		escflag;		/* Last character was escaped	*/
int		iestack[IESTACKSIZE];	/* .ie condition stack		*/
int		iestackx;		/* .ie condition stack index	*/
int		infalse;		/* True in false conditional	*/
int		lflag;			/* Landscape mode		*/
char		miscbuf[MSCSIZE];	/* Miscellaneous buffer		*/
int		nbrflag;		/* Don't allow command to break	*/
unsigned	npn;			/* Next page number		*/
int		n_reg;			/* .n register			*/
int		oldbracelevel;		/* Old level of \{ \} nesting	*/
int		oldpof;			/* Old page offset		*/
unsigned	pct;			/* Page counter			*/
int		pflag;			/* Generate PostScript output	*/
unsigned	pgl;			/* Page length			*/
int		pof;			/* Page offset			*/
SPECIAL		*spc_list;		/* Special characters		*/
int		svs;			/* Saved space			*/
FILE		*tmp;			/* Temp file pointer		*/
unsigned long	tmpseek;		/* Pointer into temp file	*/
int		T_reg;			/* .T register			*/
int		varsp;			/* Variable spacing		*/
int		xflag;			/* Suppress page eject on exit	*/
#if	ZKLUDGE
int		Zflag;			/* PS save/restore kludge	*/
#endif

/* Code global, declared in code.h. */
CODE		codeval;

/* Diversion globals, declared in div.h. */
DIV		*cdivp;			/* Pointer to diversion stack	*/
DIV		*mdivp;			/* Pointer to main diversion	*/

/* Environment globals, declared in env.h. */
ENV		env;			/* Current environment		*/
int		envinit[ENVSIZE];	/* If initialized		*/
int		envs;			/* Environment stack index	*/
int		envstak[ENVSTACK];	/* Environment stack		*/

/* Hyphenation globals, declared in hyphen.h. */
char		hindbuf[WORSIZE];
char		hletbuf[WORSIZE];
char		hyphbuf[WORSIZE];

/* Register globals, declared in reg.h. */
REG		*nrctreg;		/* Character type		*/
REG		*nrdlreg;		/* Max width of last diversion	*/
REG		*nrdnreg;		/* Height of last diversion	*/
REG		*nrdwreg;		/* Day of the week		*/
REG		*nrdyreg;		/* Day of the month		*/
REG		*nrhpreg;		/* Current horizontal place	*/
REG		*nrlnreg;		/* Output line number		*/
REG		*nrmoreg;		/* Month			*/
REG		*nrnlreg;		/* Position of last printed line */
REG		*nrpnreg;		/* Current page number		*/
REG		*nrsbreg;		/* Depth of string below base	*/
REG		*nrstreg;		/* Height of string above base	*/
REG		*nryrreg;		/* Year				*/
REG		*regt[RHTSIZE];		/* Register hash table		*/

/* Input stack global, declared in str.h. */
STR		*strp;			/* Input stack			*/

/* end of extern.c */
