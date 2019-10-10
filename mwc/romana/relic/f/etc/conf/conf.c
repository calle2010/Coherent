/*
 * The code in this file was automatically generated. Do not hand-modify!
 * Generated at Wed Aug  4 15:58:20 1993
 */

#define _KERNEL		1
#define _DDI_DKI	1

#include <sys/confinfo.h>

/* entry points for "hai" facility */

DECLARE_DRVL (scsi)


/* entry points for "at" facility */

DECLARE_DRVL (at)


/* entry points for "pty" facility */

DECLARE_DRVL (pty)


/* entry points for "rm" facility */

DECLARE_DRVL (rm)


/* entry points for "asy" facility */

DECLARE_DRVL (asy)


/* entry points for "fd" facility */

DECLARE_DRVL (fdc)


/* entry points for "vtnkb" facility */

DECLARE_DRVL (vtnkb)


/* entry points for "ct" facility */

DECLARE_DRVL (ct)


/* entry points for "nl" facility */

DECLARE_DRVL (nl)


/* entry points for "console" facility */



/* entry points for "cohmain" facility */



init_t inittab [1];

unsigned int ninit= 0;

start_t starttab [1];

unsigned int nstart= 0;

exit_t exittab [1];

unsigned int nexit= 0;

halt_t halttab [1];

unsigned int nhalt= 0;

cdevsw_t cdevsw [1];

unsigned int ncdevsw = 0;

bdevsw_t bdevsw [1];

unsigned int nbdevsw = 0;

modsw_t modsw [1];

unsigned int nmodsw = 0;

__major_t _maxmajor = 1;

__major_t _major [] = {
	NODEV, NODEV
};

__minor_t _minor [] = {
	0, 0
};

intmask_t _masktab [] = {
	0x0UL, 0x0UL, 0x0UL, 0x0UL, 
	0x0UL, 0x0UL, 0x0UL, 0x0UL, 
	0xFFFFFFFFUL
};

intr_t inttab [1];

unsigned int nintr = 0;

DRV drvl [32] = {
	DRVL_ENTRY (nl),
	DRVL_ENTRY (ct),
	DRVL_ENTRY (vtnkb),
	NULL_DRVL (),
	DRVL_ENTRY (fdc),
	DRVL_ENTRY (asy),
	NULL_DRVL (),
	NULL_DRVL (),
	DRVL_ENTRY (rm),
	DRVL_ENTRY (pty),
	NULL_DRVL (),
	DRVL_ENTRY (at),
	NULL_DRVL (),
	DRVL_ENTRY (scsi),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL (),
	NULL_DRVL ()
};

int drvn = 32;

