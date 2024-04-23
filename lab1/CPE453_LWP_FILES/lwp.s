	.file	"lwp.c"
	.comm	basePTR,8,8
	.comm	stackPTR,8,8
	.comm	temp_st_ptr,8,8
	.comm	stackInit,8,8
	.comm	lwp_ptable,960,32
	.globl	lwp_procs
	.bss
	.align 4
	.type	lwp_procs, @object
	.size	lwp_procs, 4
lwp_procs:
	.zero	4
	.globl	lwp_running
	.align 4
	.type	lwp_running, @object
	.size	lwp_running, 4
lwp_running:
	.zero	4
	.globl	scheduler
	.align 8
	.type	scheduler, @object
	.size	scheduler, 8
scheduler:
	.zero	8
	.text
	.globl	roundRobin
	.type	roundRobin, @function
roundRobin:
.LFB2:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	lwp_procs(%rip), %eax
	leal	-1(%rax), %edx
	movl	lwp_running(%rip), %eax
	cmpl	%eax, %edx
	je	.L2
	movl	lwp_running(%rip), %eax
	addl	$1, %eax
	jmp	.L3
.L2:
	movl	$0, %eax
.L3:
	movl	%eax, lwp_running(%rip)
	movl	lwp_running(%rip), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE2:
	.size	roundRobin, .-roundRobin
	.globl	new_lwp
	.type	new_lwp, @function
new_lwp:
.LFB3:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$64, %rsp
	movq	%rdi, -40(%rbp)
	movq	%rsi, -48(%rbp)
	movq	%rdx, -56(%rbp)
	movl	lwp_procs(%rip), %eax
	addl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movl	lwp_procs(%rip), %eax
	cmpl	$1, %eax
	jne	.L6
	movl	$0, lwp_running(%rip)
	jmp	.L7
.L6:
	movl	lwp_running(%rip), %eax
	addl	$1, %eax
	movl	%eax, lwp_running(%rip)
.L7:
	movl	lwp_procs(%rip), %eax
	cltq
	movq	%rax, -32(%rbp)
	movq	-56(%rbp), %rax
	movq	%rax, -16(%rbp)
	movq	$0, basePTR(%rip)
	movq	-56(%rbp), %rax
	salq	$3, %rax
	movq	%rax, %rdi
	call	malloc
	movq	%rax, stackInit(%rip)
	movq	stackInit(%rip), %rax
	movq	%rax, -24(%rbp)
	movq	stackInit(%rip), %rax
	movq	-56(%rbp), %rdx
	salq	$6, %rdx
	subq	$8, %rdx
	addq	%rdx, %rax
	movq	%rax, stackPTR(%rip)
	movq	stackPTR(%rip), %rax
	movq	-48(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	stackPTR(%rip), %rax
	subq	$8, %rax
	movq	%rax, stackPTR(%rip)
	movq	stackPTR(%rip), %rax
	movl	$3735928559, %ecx
	movq	%rcx, (%rax)
	movq	stackPTR(%rip), %rax
	subq	$8, %rax
	movq	%rax, stackPTR(%rip)
	movq	stackPTR(%rip), %rax
	movq	-40(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	stackPTR(%rip), %rax
	subq	$8, %rax
	movq	%rax, stackPTR(%rip)
	movq	stackPTR(%rip), %rax
	movq	basePTR(%rip), %rdx
	movq	%rdx, (%rax)
	movq	stackPTR(%rip), %rax
	movq	%rax, basePTR(%rip)
	movq	stackPTR(%rip), %rax
	subq	$56, %rax
	movq	%rax, stackPTR(%rip)
	movq	stackPTR(%rip), %rax
	movq	basePTR(%rip), %rdx
	movq	%rdx, (%rax)
	movq	stackPTR(%rip), %rax
	movq	%rax, -8(%rbp)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	-32(%rbp), %rdx
	movq	%rdx, (%rax)
	movq	-24(%rbp), %rdx
	movq	%rdx, 8(%rax)
	movq	-16(%rbp), %rdx
	movq	%rdx, 16(%rax)
	movq	-8(%rbp), %rdx
	movq	%rdx, 24(%rax)
	movq	-32(%rbp), %rax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE3:
	.size	new_lwp, .-new_lwp
	.globl	lwp_start
	.type	lwp_start, @function
lwp_start:
.LFB4:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, lwp_running(%rip)
#APP
# 102 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 102 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 102 "lwp.c" 1
	pushq %rbp
# 0 "" 2
# 103 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movq	%rax, temp_st_ptr(%rip)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+16, %rax
	movq	8(%rax), %rax
#APP
# 104 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 105 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 105 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 105 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE4:
	.size	lwp_start, .-lwp_start
	.globl	lwp_stop
	.type	lwp_stop, @function
lwp_stop:
.LFB5:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 113 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 113 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 113 "lwp.c" 1
	pushq %rbp
# 0 "" 2
#NO_APP
	movl	lwp_running(%rip), %edx
#APP
# 114 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable+16, %rdx
	movq	%rax, 8(%rdx)
	movq	temp_st_ptr(%rip), %rax
#APP
# 115 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 116 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 116 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 116 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE5:
	.size	lwp_stop, .-lwp_stop
	.globl	lwp_getpid
	.type	lwp_getpid, @function
lwp_getpid:
.LFB6:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	lwp_running(%rip), %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE6:
	.size	lwp_getpid, .-lwp_getpid
	.globl	lwp_yield
	.type	lwp_yield, @function
lwp_yield:
.LFB7:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
#APP
# 132 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 132 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 132 "lwp.c" 1
	pushq %rbp
# 0 "" 2
#NO_APP
	movl	lwp_running(%rip), %edx
#APP
# 133 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable+16, %rdx
	movq	%rax, 8(%rdx)
	movq	scheduler(%rip), %rax
	testq	%rax, %rax
	je	.L14
	movq	scheduler(%rip), %rax
	jmp	.L15
.L14:
	movl	$roundRobin, %eax
.L15:
	movq	%rax, scheduler(%rip)
	movq	scheduler(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+16, %rax
	movq	8(%rax), %rax
#APP
# 138 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 139 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 139 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 139 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	lwp_yield, .-lwp_yield
	.globl	lwp_set_scheduler
	.type	lwp_set_scheduler, @function
lwp_set_scheduler:
.LFB8:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movq	%rdi, -8(%rbp)
	movq	-8(%rbp), %rax
	movq	%rax, scheduler(%rip)
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	lwp_set_scheduler, .-lwp_set_scheduler
	.globl	lwp_exit
	.type	lwp_exit, @function
lwp_exit:
.LFB9:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
#APP
# 155 "lwp.c" 1
	pushq %rax
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rbx
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rcx
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rdx
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rsi
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rdi
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r8
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r9
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r10
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r11
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r12
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r13
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r14
# 0 "" 2
# 155 "lwp.c" 1
	pushq %r15
# 0 "" 2
# 155 "lwp.c" 1
	pushq %rbp
# 0 "" 2
#NO_APP
	movl	lwp_running(%rip), %edx
#APP
# 156 "lwp.c" 1
	movq  %rsp,%rax
# 0 "" 2
#NO_APP
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable+16, %rdx
	movq	%rax, 8(%rdx)
	movl	lwp_procs(%rip), %eax
	leal	-1(%rax), %edx
	movl	lwp_running(%rip), %eax
	cmpl	%eax, %edx
	jne	.L18
	movl	lwp_procs(%rip), %eax
	cmpl	$1, %eax
	jle	.L18
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free
	movl	lwp_procs(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movq	scheduler(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+16, %rax
	movq	8(%rax), %rax
#APP
# 162 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 163 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 163 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 163 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	jmp	.L17
.L18:
	movl	lwp_procs(%rip), %eax
	cmpl	$1, %eax
	jle	.L20
	movl	lwp_procs(%rip), %eax
	leal	-1(%rax), %edx
	movl	lwp_running(%rip), %eax
	cmpl	%eax, %edx
	je	.L20
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free
	movl	lwp_running(%rip), %eax
	addl	$1, %eax
	movl	%eax, -4(%rbp)
	jmp	.L21
.L22:
	movl	-4(%rbp), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	(%rax), %rax
	leaq	-1(%rax), %rdx
	movl	-4(%rbp), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	%rdx, (%rax)
	movl	-4(%rbp), %eax
	subl	$1, %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movl	-4(%rbp), %edx
	movslq	%edx, %rdx
	salq	$5, %rdx
	addq	$lwp_ptable, %rdx
	movq	(%rdx), %rcx
	movq	%rcx, (%rax)
	movq	8(%rdx), %rcx
	movq	%rcx, 8(%rax)
	movq	16(%rdx), %rcx
	movq	%rcx, 16(%rax)
	movq	24(%rdx), %rdx
	movq	%rdx, 24(%rax)
	addl	$1, -4(%rbp)
.L21:
	movl	lwp_procs(%rip), %eax
	cmpl	%eax, -4(%rbp)
	jl	.L22
	movl	lwp_procs(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movq	scheduler(%rip), %rax
	cmpq	$roundRobin, %rax
	jne	.L23
	movq	scheduler(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
	movl	lwp_running(%rip), %eax
	testl	%eax, %eax
	jne	.L24
	movl	lwp_procs(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_running(%rip)
	jmp	.L26
.L24:
	movl	lwp_running(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_running(%rip)
	jmp	.L26
.L23:
	movq	scheduler(%rip), %rax
	call	*%rax
	movl	%eax, lwp_running(%rip)
.L26:
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable+16, %rax
	movq	8(%rax), %rax
#APP
# 188 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 189 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 189 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 189 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
	jmp	.L17
.L20:
	movl	lwp_procs(%rip), %eax
	cmpl	$1, %eax
	jne	.L17
	movl	lwp_running(%rip), %eax
	cltq
	salq	$5, %rax
	addq	$lwp_ptable, %rax
	movq	8(%rax), %rax
	movq	%rax, %rdi
	call	free
	movl	lwp_procs(%rip), %eax
	subl	$1, %eax
	movl	%eax, lwp_procs(%rip)
	movq	temp_st_ptr(%rip), %rax
#APP
# 194 "lwp.c" 1
	movq  %rax,%rsp
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rbp
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r15
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r14
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r13
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r12
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r11
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r10
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r9
# 0 "" 2
# 195 "lwp.c" 1
	popq  %r8
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rdi
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rsi
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rdx
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rcx
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rbx
# 0 "" 2
# 195 "lwp.c" 1
	popq  %rax
# 0 "" 2
# 195 "lwp.c" 1
	movq  %rbp,%rsp
# 0 "" 2
#NO_APP
.L17:
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE9:
	.size	lwp_exit, .-lwp_exit
	.ident	"GCC: (GNU) 4.8.5 20150623 (Red Hat 4.8.5-44)"
	.section	.note.GNU-stack,"",@progbits
