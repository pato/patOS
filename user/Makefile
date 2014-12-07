PROGS = shell ls shutdown echo cat clear welcome exit

all : $(PROGS)

-include ../common.mak

shell : CFILES=shell.c libc.c heap.c

echo : CFILES=echo.c libc.c heap.c

ls : CFILES=ls.c libc.c heap.c

shutdown : CFILES=shutdown.c libc.c heap.c

exit : CFILES=exit.c libc.c heap.c

cat : CFILES=cat.c libc.c heap.c

clear : CFILES=clear.c libc.c heap.c

welcome : CFILES=welcome.c libc.c heap.c

$(PROGS) : % : Makefile $(OFILES)
	ld -N -m elf_i386 -e start -Ttext=0x80000000 -o $@ $(OFILES)

clean ::
	rm -f $(PROGS)
