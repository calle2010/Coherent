#include <errno.h>
#include <fcntl.h>

static char PtyName[32] = "/dev/ptyXY";
static char TtyName[32] = "/dev/ttyXY";

/*
 * Claim a pseudo-tty.
 * Takes pointers to buffers for the names of the newly claimed pair
 * and returns a file descriptor on the open pty.
 *
 * Returns -1 on any sort of failure.  errno will be set to ENXIO
 * if there were no more ptys.
 */
int
getpseudotty(newtty, newpty)
	char *newtty, *newpty;
{
	register char *l, *d;
	int X, Y, f;
   
	/*
	 * Locate the "XY" in the above strings.
	 */
	X = strlen(PtyName) - 2;
	Y = X + 1;

	/*
	 * Walk through all possible values of X.
	 */
	for (l = "pqrstuvwxyz"; '\0' != *l; ++l) {
		PtyName[X] = *l;
		/*
		 * Walk through all possible values of Y.
		 */
		for (d = "0123456789abcdef"; *d; ++d) {
			PtyName[Y] = *d;
			/*
			 * If we can open() the pty, it means we have
			 * claimed it.
			 */
			if ((f = open(PtyName, O_RDWR)) != -1) {
				/*
				 * Return the name of the pty.
				 */
				strcpy(newpty, PtyName);
				/*
				 * Build and return the name of the tty.
				 */
				TtyName[X] = *l;
				TtyName[Y] = *d;
				strcpy(newtty, TtyName);
				return f;
			} else if(errno == ENXIO) {
				/*
				 * No more ptys!
				 */
				break;
			}
		}
	}
	return -1;
}
