	.file	"testlong.c"
	.text
	.p2align 4,,15
.globl main
	.type	main, @function
main:
	pushl	%ebp
	movl	%esp, %ebp
	pushl	%ebx
	subl	$16, %esp
	movl	w, %ebx
	testl	%ebx, %ebx
	jle	.L6
	xorl	%edx, %edx
	.p2align 4,,7
	.p2align 3
.L3:
	movl	-12(%ebp), %ecx
	movl	%ecx, 0(,%edx,4)
	addl	$1, %edx
	cmpl	%ebx, %edx
	jne	.L3
	xorl	%edx, %edx
	.p2align 4,,7
	.p2align 3
.L4:
	movzbl	-5(%ebp), %ecx
	addl	$1, %edx
	movb	%cl, (%eax)
	movzbl	-6(%ebp), %ecx
	movb	%cl, 1(%eax)
	movzbl	-7(%ebp), %ecx
	movb	%cl, 2(%eax)
	addl	$3, %eax
	cmpl	%edx, w
	jg	.L4
.L6:
	addl	$16, %esp
	popl	%ebx
	popl	%ebp
	ret
	.size	main, .-main
	.comm	w,4,4
	.ident	"GCC: (Ubuntu 4.4.1-4ubuntu8) 4.4.1"
	.section	.note.GNU-stack,"",@progbits
