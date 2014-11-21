
	.global start
start:

	.extern heap_init
	call heap_init

	.extern main
	call main

	push %eax
loop:
	call exit
	jmp loop
