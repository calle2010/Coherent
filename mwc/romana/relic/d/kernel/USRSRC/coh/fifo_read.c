/* fifo_read.c -- Read from an internally typed fifo.  */
#include <sys/typed.h>

/* Read a typed space from a fifo.
 * Return a pointer to the next typed space in the fifo ffp.  Returns
 * NULL on end of fifo.
 *
 * This read assumes that ffp->f_space has type T_FIFO_SIC.
 */
typed_space *
fifo_read(ffp)
	register FIFO *ffp;
{
	typed_space *retval;

	/* Read MUST be set.  */
	if (F_READ != F_READ & ffp->f_flags ) {
		return(T_NULL);  /* This ffp is not open for reading.  */
	}

	/* From here to the end of fifo_read is really fifo_read_sic().  */


	/* Space of size 0 marks EOFIFO.  */
	if ((long)0 == ffp->f_offset->ts_size) {
		retval = T_NULL;
	} else {
		/* Return the next space.  */
		retval = ffp->f_offset;
		/* Advance to the next space.  */
		(char *) ffp->f_offset += ffp->f_offset->ts_size;
	}

	return(retval);
} /* fifo_read() */
