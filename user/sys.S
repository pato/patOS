	#
	# user-side system calls
	#
	# System calls use a special convention:
        #     %eax  -  system call number
        #     %ecx  -  first argument
        #     %edx  -  second argument
        #
        # more arguments could be passed in memory
        #

	# void exit(int status)
	.global exit
exit:
	mov $0,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# void putchar(int ch)
	.global putchar
putchar:
	mov $1,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long fork()
	.global fork
fork:
	push %ebx
	push %esi
	push %edi
	push %ebp
	mov $2,%eax
	mov $0,%ecx
	mov $0,%edx
	int $100
	pop %ebp
	pop %edi
	pop %esi
	pop %ebx
	ret

	# long semaphore(long n)
	.global semaphore
semaphore:
	mov $3,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long down(long sem)
	.global down
down:
	mov $4,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long up(long sem)
	.global up
up:
	mov $5,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long join(long proc)
	.global join
join:
	mov $6,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long shutdown()
	.global shutdown
shutdown:
	mov $7,%eax
	mov $0,%ecx
	mov $0,%edx
	int $100
	ret

	# long open(char* name)
	.global open
open:
	mov $8,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long getlen(long file)
	.global getlen
getlen:
	mov $9,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# long read(long file, void* buf, long len)
	.global read
read:
	mov $10,%eax
	lea 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# void seek(long file, long pos)
	.global seek
seek:
	mov $11,%eax
	mov 4(%esp),%ecx
	mov 8(%esp),%edx
	int $100
	ret

	# void close(long file)
	.global close
close:
	mov $12,%eax
	mov 4(%esp),%ecx
	mov $0,%edx
	int $100
	ret

	# void execv(char* name, char** args)
	.global execv
execv:
	mov $13,%eax
	mov 4(%esp),%ecx
	mov 8(%esp),%edx
	int $100
	ret

	# long getchar()
	.global getchar
getchar:
	mov $14,%eax
	int $100
	ret

