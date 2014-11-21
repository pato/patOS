
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
