#define ASSEMBLY

	# syscallTrap()
	# the user implicitly passes 3 arguments in %eax, %ecx, %edx
	# %eax is syscall number
	# the rest of the arguments depend on %eax
	.global syscallTrap
syscallTrap:
	push %ds

	push %ebp
	push %edi
	push %esi
	push %ebx

	push %edx
	push %ecx
	push %eax

	mov %esp,%eax                         // why?
	push %eax

/*
    Arguments:

            context
context->   syscall #               
            a0 
            a1
            user %ebx
            user %esi
            user %edi
            user %ebp
            user %ds
            user %eip
            user %cs
            user %eflags
            user %esp
            user %ss

*/


	mov kernelDataSeg,%eax
	movw %ax,%ds

	.extern syscallHandler
	call syscallHandler

	add $32,%esp

	pop %ds
	iret


	# switchToUser(pc,esp,eax)
	.global switchToUser
switchToUser:
	mov 4(%esp),%ecx	# pc
	mov 8(%esp),%ebx        # esp
	mov 12(%esp),%eax       # eax

	mov userDataSeg,%edx    # for user SS = DS
	push %edx               # ring 3 SS

	push %ebx               # user ESP

	pushf                   # ring 3 flags
        orl $0x200,(%esp)       # enable interrupts

	mov userCodeSeg,%edx
        push %edx               # ring 3 CS

	mov userDataSeg,%edx
	
	push %ecx               # user EIP

	movw %dx,%ds            # no more memory refs until iret

        iret



	# oldval = cs32(uint32_t* ptr, uint32_t ifval, uint32_t thenval)
	.global cs32
cs32:
	mov 4(%esp),%ecx       # ptr
	mov 8(%esp),%eax       # ifval
	mov 12(%esp),%edx      # thenval
        lock cmpxchgl %edx,(%ecx)
	ret                    # looks strange but always returns the oldval
                               #   - if it worked, %eax had the ifval which
                               #     is equals to the oldval
                               #   - if it failed, %eax will be loaded with
                               #     the oldval
	
	# contextSwitch(long* currentESP, long nextESP, long eflags)
        .global contextSwitch
contextSwitch:
        mov 4(%esp),%eax     // *currentESP
        mov 8(%esp),%ecx     // nextESP
	mov 12(%esp),%edx    // eflags

	cmp $0,%eax
	jz activateNext

        push %ebx
        push %esi
        push %edi
        push %ebp

        mov %esp,(%eax)

activateNext:
        mov %ecx,%esp

        pop %ebp
        pop %edi
        pop %esi
        pop %ebx

	// get the new flags, mostly used for interrupt flag
	push %edx
	popf

        ret

	# uint32_t fetchAndAdd32(uint32_t* ptr, uint32_t v)
	.global fetchAndAdd32
fetchAndAdd32:
	mov 4(%esp),%ecx		# ptr
	mov 8(%esp),%eax		# ptr
	lock xaddl %eax,(%ecx)
	ret

        # eflags
        .global eflags
eflags:
	pushf
	pop %eax
	ret

	# outb(int port, int val)
	.global outb
outb:
	push %edx
	mov 8(%esp),%dx
	mov 12(%esp),%al
	outb %al,%dx
	pop %edx
	ret

	# int inb(int port)
	.global inb
inb:
	push %edx
	mov 8(%esp),%dx
	inb %dx,%al
	pop %edx
	and $0xff,%eax
	ret

	# unsigned long inb(int port)
	.global inl
inl:
	push %edx
	mov 8(%esp),%dx
	inl %dx,%eax
	pop %edx
	ret


	#
	# void ltr(uint32_t tr)
	#
	.global ltr
ltr:
	mov 4(%esp),%eax
	ltr %ax
	ret

	/* vmm_on(pd) */
	.global vmm_on
vmm_on:
	mov 4(%esp),%eax
	mov %eax,%cr3

	mov %cr0,%eax
	or $0x80000000,%eax
	mov %eax,%cr0
	ret
	
	.global getcr0
getcr0:
	mov %cr0,%eax
	ret
	
	.global getcr3
getcr3:
	mov %cr3,%eax
	ret

	.global invlpg
invlpg:
	mov 4(%esp),%eax
	invlpg (%eax)
	ret

	.global cli
cli:
	cli
	ret

	.global sti
sti:
	sti
	ret

	.global irq0
irq0:
	push %eax
	mov $0, %eax
	jmp irq_common

	.global irq1
irq1:
	push %eax
	mov $1, %eax
	jmp irq_common

	.global irq2
irq2:
	push %eax
	mov $2, %eax
	jmp irq_common

	.global irq3
irq3:
	push %eax
	mov $3, %eax
	jmp irq_common

	.global irq4
irq4:
	push %eax
	mov $4, %eax
	jmp irq_common

	.global irq5
irq5:
	push %eax
	mov $5, %eax
	jmp irq_common

	.global irq6
irq6:
	push %eax
	mov $6, %eax
	jmp irq_common

	.global irq7
irq7:
	push %eax
	mov $7, %eax
	jmp irq_common

	.global irq8
irq8:
	push %eax
	mov $8, %eax
	jmp irq_common

	.global irq9
irq9:
	push %eax
	mov $9, %eax
	jmp irq_common

	.global irq10
irq10:
	push %eax
	mov $10, %eax
	jmp irq_common

	.global irq11
irq11:
	push %eax
	mov $11, %eax
	jmp irq_common

	.global irq12
irq12:
	push %eax
	mov $12, %eax
	jmp irq_common

	.global irq13
irq13:
	push %eax
	mov $13, %eax
	jmp irq_common

	.global irq14
irq14:
	push %eax
	mov $14, %eax
	jmp irq_common

	.global irq15
irq15:
	push %eax
	mov $15, %eax
	jmp irq_common

irq_common:
	push %ebx
	push %ecx
	push %edx
	push %esi
	push %edi
	push %ebp

	push %ds
	mov kernelDataSeg,%ecx
	mov %cx,%ds

	mov %cr2,%ebp
	push %ebp

	push %eax

	.extern pic_irq
	call pic_irq

	add $4,%esp    /* pop argument */

	pop %ebp
	mov %ebp,%cr2

	pop %ds

	pop %ebp
	pop %edi
	pop %esi
	pop %edx
	pop %ecx
	pop %ebx
	pop %eax
	iret

# PIT

	/* pit_init(divide) */
	.global pit_do_init
pit_do_init:
	pushf			# push IF
	cli			# disable interrupts
	movb $0b00110100,%al	# 00 (channel 0)
				# 110 (lobyte/hibyte)
				# 100 (rate generator)
	outb %al,$0x43		# write command
	movb 8(%esp),%al	# divide
        outb %al,$0x40
	movb 9(%esp),%al
	outb %al,$0x40
	popf			# pop IF
	ret


	.global pageFaultHandler
pageFaultHandler:
        push %eax
        push %ebx
        push %ecx
        push %edx
        push %esi
        push %edi
        push %ebp

	/* switch to kernel DS */
	push %ds
	mov kernelDataSeg,%eax
	mov %ax,%ds


        mov %cr2,%eax   /* address */
        push %eax

	mov %esp, %eax
	push %eax

        .extern vmm_pageFault
        call vmm_pageFault

        add $8,%esp    /* pop argument */

        /* restore DS */
        pop %ds

        pop %ebp
        pop %edi
        pop %esi
        pop %edx
        pop %ecx
        pop %ebx
        pop %eax
        add $4,%esp   /* pop error */
        iret

	/* memset(void* p, int val, size_t sz) */
	.global memset
memset:
	mov 4(%esp),%eax	# p
	mov 8(%esp),%ecx	# val
	mov 12(%esp),%edx	# sz

1:
	add $-1,%edx
	jl 1f
	movb %cl,(%eax,%edx,1)
	jmp 1b

1:
	ret


	/* memcpy(void* dest, void* src, size_t n) */
	.global memcpy
memcpy:
	mov 4(%esp),%eax       # dest
        mov 8(%esp),%edx       # src
        mov 12(%esp),%ecx      # n
	push %ebx
1:
	add $-1,%ecx
	jl 1f
	movb (%edx),%bl
	movb %bl,(%eax)
	add $1,%edx
	add $1,%eax
	jmp 1b
1:
	pop %ebx
	ret
