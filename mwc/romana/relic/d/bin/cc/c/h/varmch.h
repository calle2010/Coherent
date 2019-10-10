/*
 * This header file defines the machine specific variant bits
 * for all machines so the cc dispatcher can run all compilers
 * with one compiled binary.
 */

/*
 * Intel iAPX-86.
 */
#define	VSMALL	(VMBASE+0)		/* SMALL model			*/
#define	VCOMPAC	(VMBASE+1)		/* COMPACT model - UNIMPLEMENTED */
#define	VMEDIUM	(VMBASE+2)		/* MEDIUM model - UNIMPLEMENTED	*/
#define	VLARGE	(VMBASE+3)		/* LARGE model			*/
#define	V8087	(VMBASE+4)		/* 8087 floating point - UNIMPLEMENTED */
#define	VRAM	(VMBASE+5)		/* Daisy code->data option	*/
#define	VOMF	(VMBASE+6)		/* Generate OMF objects		*/
#define	V80186	(VMBASE+7)		/* Generate 80186 instructions	*/
#define	V80287	(VMBASE+8)		/* Generate 80287 floating point */
#define	VALIGN	(VMBASE+9)		/* Word-align stack		*/
#define	VEMU87	(VMBASE+10)		/* Generate 8087 emulation calls */

/*
 * Motorola 68000.
 */
#define VSPLIM	(VMBASE+0)	/* Stack limit checks via trapv */
#define	VNOTRAPS (VMBASE+1)	/* Inhibit trap generation under GEMDOS */

/*
 * Zilog Z8001.
 */
