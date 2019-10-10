#if	! __BORLANDC__
#error	This file is for Borland C++ support only.
#endif

#define	_DDI_DKI	1
#define	_SYSV4		1

#include <kernel/ddi_glob.h>
#include <kernel/ddi_cpu.h>
#include <kernel/x86lock.h>
#include <kernel/defer.h>
#include <sys/confinfo.h>
#include <stdlib.h>
#include <dos.h>

__EXTERN_C_BEGIN__

void		STREAMS_TIMEOUT	__PROTO ((void));

__EXTERN_C_END__


/*
 * Clock-interrupt handler and friends.
 */

static void clock_deferred_a (void) {
	(* (char __far *) MK_FP (0xB800, 1)) ++;
}
static void clock_deferred_b (void) {
	(* (char __far *) MK_FP (0xB800, 2)) ++;
}
static void clock_deferred_c (void) {
	(* (char __far *) MK_FP (0xB800, 3)) ++;
}

__EXTERN_C__
void clock_intr (int __NOTUSED (unused)) {
	(* (char __far *) MK_FP (0xB800, 0)) ++;
	defer_int_cpu (clock_deferred_c, 0);
	defer_int_here (clock_deferred_a);
	defer_int_any (clock_deferred_b);
	defer_int_any (STREAMS_TIMEOUT);
}

/*
 * Emulate the Coherent vector-manipulation routines in the DOS environment.
 */

__EXTERN_C__
void SETIVEC (int num) {
	intthunk_t	prevint;
	intmask_t	mask = __GET_BASE_MASK ();
	int		vect = inttab [num].int_vector;
	int		newmask = 1 << (vect & 7);

	ASSERT (vect != 2);
	ASSERT_BASE_LEVEL ();

	prevint = getvect (vect < 8 ? vect + 8 : vect + 0x70);

	__emit__ (__CLI__);

	setvect (vect < 8 ? vect + 8 : vect + 0x70, inttab [num].int_thunk);

	mask = ddi_cpu_data ()->dc_base_mask;

	/*
	 * Set up for chaining if the interrupt was previously enabled.
	 */

	if ((mask & newmask) == 0)
		(* inttab [num].int_prev) = prevint;
	else {

		mask &= ~ newmask;
		__SET_BASE_MASK (mask);
	}

	__emit__ (__STI__);
}


/*
 * Note that the way our interrupt nesting system works, it would be a Bad
 * Idea to try and clear an interrupt vector from an interrupt service
 * routine.
 */

__EXTERN_C__
void CLRIVEC (int num) {
	int		vect = inttab [num].int_vector;
	int		newmask = 1 << (vect & 7);
	intmask_t	mask = __GET_BASE_MASK ();

	ASSERT_BASE_LEVEL ();

	__emit__ (__CLI__);

	if (* inttab [num].int_prev != NULL)
		setvect (vect < 8 ? vect + 8 : vect + 0x70,
			 * inttab [num].int_prev);
	else {

		mask |= newmask;
		__SET_BASE_MASK (mask);
	}

	__emit__ (__STI__);
}
