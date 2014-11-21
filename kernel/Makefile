all : kernel.img

-include ../common.mak

kernel : Makefile $(OFILES)
	ld -N -m elf_i386 -e start --section-start mbr=0x7c00 -Ttext=0x9000 -o kernel $(OFILES)

clean ::
	rm -f kernel
