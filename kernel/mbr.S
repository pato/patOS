/*
    This is the bootstrap code.

    It will be stored in sector#0 of the boot drive but the BIOS will load
    it at address 0x7c0

    We will compile the MBR at 0x7C00 then extract the appropriate 512 byte
    block and copy it to the boot drive

    
*/
   
        .code16
	.section mbr,"xa"

	.global start
start:
	cli			# disable interrupts

	mov $0,%ax		# initialize data and stack segments
	mov %ax,%ds
	mov %ax,%ss

	movb %dl,0x7000		# save boot drive id

	# print an 'x' to let the world know we're here
	movb $'x',%al
	movb $1,%ah
	mov $0,%dx
	int $0x14

	# read the drive parameters
	movw $30,0x6000
	movb $0x48,%ah		# function code (read drive parameters)
	movb 0x7000,%dl		# drive index
	movw $0x6000,%si	# result buffer
	int $0x13		# call the BIOS

	movw 0x6010,%cx		# low 16 bits of sector count
				# this limits hdd size to 64k * 512 = 32MB


	# read hdd into memory starting at loadKernelHere
	movw $0x6000,%si	# DAP pointer
	movb $16,(%si)		# size of buffer
	movb $0,1(%si)		# unused
	add $-1,%cx		# -1 sector
	movw %cx,2(%si)		# number of sectors
	movw $loadKernelHere,4(%si)	# buffer offset
	movw $0,6(%si)		# buffer segment
	movw $1,8(%si)		# starting sector number
	movw $0,10(%si)
	movw $0,12(%si)
	movw $0,14(%si)

	mov $0x42,%ah		# function code
	movb 0x7000,%dl		# drive index
	int $0x13		# read the sectors

	# load the gdt
	lgdt gdtDesc

	# load the idt
	lidt idtDesc

	# enable protected mode, still running in 16 bit segment with
        # original segments
	mov %cr0,%eax
	or $1,%eax
	mov %eax,%cr0


	# set the stack and data segments, can't touch memory until
	# we get into 32 bit mode
	#mov $16,%ax
	#mov %ax,%ds
	#mov %ax,%ss

	# do a far jump to the kernel entry point in 32 bit mode
	ljmp $8,$loadKernelHere


end:
	.skip 510 - (end - start)
	.byte 0x55
	.byte 0xaa

	.code32
loadKernelHere:
	mov $16,%ax
	mov %ax,%ss
	mov $16,%ax
	mov %ax,%ds
	jmp kStart

#define GDT_COUNT 6

gdt:
	.long 0			# gdt[0] must be empty
	.long 0

	.long 0x0000ffff	# gdt[1] KERNEL CODE
	.long 0x00cf9800

	.long 0x0000ffff	# gdt[2] KERNEL DATA
	.long 0x00cf9200

	.long 0x0000ffff	# gdt[3] USER CODE
	.long 0x00cff800

	.long 0x0000ffff	# gdt[4] USER DATA
	.long 0x00cff200

	.global tssDescriptor
tssDescriptor:
	.long 0			# gdt[5] TSS
	.long 0

gdtDesc:
	.word (GDT_COUNT * 8) - 1
	.long gdt

	.global kernelCodeSeg
kernelCodeSeg:
	.long 1 * 8

	.global kernelDataSeg
kernelDataSeg:
	.long 2 * 8

	.global userCodeSeg
userCodeSeg:
	.long 3 * 8 + 3

	.global userDataSeg
userDataSeg:
	.long 4 * 8 + 3

	.global tssDS
tssDS:
	.long 5 * 8




#define IDT_COUNT 256

	.global idt
idt:
	.skip IDT_COUNT * 8
idtDesc:
	.word (IDT_COUNT * 8) - 1
	.long idt

