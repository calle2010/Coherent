//////////
/ libc/string/i386/memset.s
/ i386 C string library.
/ ANSI 4.11.6.1.
//////////

//////////
/ void *
/ memset(void *String, int Char, size_t Count)
/
/ Set Count bytes of String to Char.
/ Uses repeated dword copy for efficiency.
//////////

String	.equ	8
Char	.equ	String+4
Count	.equ	Char+4

	.data
fourx:	.long	0x01010101

	.text
	.globl	memset

memset:
	push	%edi

	movl	%ecx, Count(%esp)	/ Count to ECX
	movl	%edi, String(%esp)	/ String address to EDI
	movzxb	%eax, Char(%esp)	/ Char to EAX
	mull	%eax, fourx		/ Char:Char:Char:Char in EAX
	movl	%edx, %ecx		/ Save Count in EDX
	shrl	%ecx, $2		/ Count/4
	cld
	rep
	stosl				/ Copy Count/4 Char dwords to String
	jnc	?byte			/ CF contains Count bit 1 from shrl above
	stosw				/ Copy a word

?byte:
	shrl	%edx, $1
	jnc	?done
	stosb				/ Copy Char to String

?done:
	movl	%eax, String(%esp)	/ Return the destination in EAX

	pop	%edi
	ret

/ end of libc/string/i386/memset.s
