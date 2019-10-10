////////
/
/ Intel 8086 floating point.
/ assigned subtract from a double float.
/ small model.
/
////////

	.globl	ddlsub
	.globl	ddrsub
	.globl	_fpac_
	.globl	dlsub

////////
/
/ ** ddlsub -- assigned double subtract (lvalue)
/ ** ddrsub -- assigned double subtract (rvalue)
/
/ these two routines are called directly by the compiler to do double
/ floating assigned subtract. these small routines make the  compiler
/ simpler, and get rid of a very bulky code sequence that would  have
/ to be generated for a fairly uncommon operation.
/
/ compiler calling sequences:
/	push	<right double rvalue>
/	lea	ax,<left double lvalue>
/	push	ax
/	call	ddrsub
/	add	sp,10
/
/	lea	ax,<right double lvalue>
/	push	ax
/	lea	ax,<left double lvalue>
/	push	ax
/	call	ddlsub
/	add	sp,4
/
/ outputs:
/	_fpac_=result.
/
/ uses:
/	ax, bx, cx, dx
/
////////

pa	=	8			/ pointer to left
pb	=	10			/ pointer to right
b	=	10			/ right

ddlsub:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ entry

	mov	ax,pb(bp)		/ ax=pointer to right operand
	jmp	l0			/ finish in common code

ddrsub:	push	si			/ standard
	push	di			/ c
	push	bp			/ function
	mov	bp,sp			/ entry

	lea	ax,b(bp)		/ ax=pointer to right operand

l0:	push	ax			/ second arg is right lvalue.
	mov	si,pa(bp)		/ si=pointer to left.
	push	6(si)			/ push
	push	4(si)			/ the
	push	2(si)			/ left
	push	0(si)			/ rvalue.
	call	dlsub			/ do the subtract.
	add	sp,$10			/ pop args

	mov	ax,_fpac_+6		/ copy
	mov	6(si),ax		/ the
	mov	ax,_fpac_+4		/ result
	mov	4(si),ax		/ back
	mov	ax,_fpac_+2		/ to
	mov	2(si),ax		/ the
	mov	ax,_fpac_+0		/ left
	mov	0(si),ax		/ operand.

	pop	bp			/ standard
	pop	di			/ c
	pop	si			/ function
	ret				/ return.
