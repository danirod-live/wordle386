FILES = nativeos/hardware/kbd.c \
	nativeos/hardware/vgafb.c \
	nativeos/hardware/port.c \
	nativeos/hardware/rtclock.c \
	nativeos/hardware/spinlock.c \
	nativeos/hardware/pctimer.c \
	nativeos/stdkern/strcat.c \
	nativeos/stdkern/strchr.c \
	nativeos/stdkern/ringbuf.c \
	nativeos/stdkern/list.c \
	nativeos/stdkern/strlen.c \
	nativeos/stdkern/memcpy.c \
	nativeos/stdkern/strcmp.c \
	nativeos/stdkern/memset.c \
	nativeos/stdkern/strsep.c \
	nativeos/stdkern/strcpy.c \
	nativeos/stdkern/strdup.c \
	nativeos/vfs/fs_devfs.c \
	nativeos/vfs/fs_fsops.c \
	nativeos/vfs/fs_path.c \
	nativeos/vfs/fs_vfs.c \
	nativeos/i386/multiboot.S \
	nativeos/i386/kernel/cpu/idt.c \
	nativeos/i386/kernel/cpu/lidt.S \
	nativeos/i386/kernel/mem/alloc.c \
	nativeos/i386/kernel/mem/heap.c \
	nativeos/i386/kernel/mem/pmm.c \
	wordle.c \
	kern_main.c \
	locore.S

OBJECTS = nativeos/hardware/kbd.o \
	nativeos/hardware/vgafb.o \
	nativeos/hardware/port.o \
	nativeos/hardware/rtclock.o \
	nativeos/hardware/spinlock.o \
	nativeos/hardware/pctimer.o \
	nativeos/stdkern/strcat.o \
	nativeos/stdkern/strchr.o \
	nativeos/stdkern/ringbuf.o \
	nativeos/stdkern/list.o \
	nativeos/stdkern/strlen.o \
	nativeos/stdkern/memcpy.o \
	nativeos/stdkern/strcmp.o \
	nativeos/stdkern/memset.o \
	nativeos/stdkern/strsep.o \
	nativeos/stdkern/strcpy.o \
	nativeos/stdkern/strdup.o \
	nativeos/vfs/fs_devfs.o \
	nativeos/vfs/fs_fsops.o \
	nativeos/vfs/fs_path.o \
	nativeos/vfs/fs_vfs.o \
	nativeos/i386/multiboot.o \
	nativeos/i386/kernel/cpu/idt.o \
	nativeos/i386/kernel/cpu/lidt.o \
	nativeos/i386/kernel/mem/alloc.o \
	nativeos/i386/kernel/mem/heap.o \
	nativeos/i386/kernel/mem/pmm.o \
	wordle.o \
	kern_main.o \
	locore.o


wordle.elf: $(OBJECTS)
	ld.lld -nostdlib -T kernel.ld $(OBJECTS) -o wordle.elf

GRUB_ROOT = $(shell dirname `which grub-mkrescue`)/..
wordle.iso: wordle.elf
	rm -rf dist
	mkdir dist
	cp -R cdrom/* dist
	cp wordle.elf dist/boot/wordle.elf
	grub-mkrescue -d $(GRUB_ROOT)/lib/grub/i386-pc -o wordle.iso dist

qemu: wordle.iso
	qemu-system-i386 -cdrom wordle.iso

%.o: %.c
	clang -c -o $@ -nostdlib -ffreestanding -fno-builtin -target i386-elf -m32 -I. $<

%.o: %.S
	clang -c -o $@ -nostdlib -ffreestanding -fno-builtin -target i386-elf -m32 -I. $<

clean:
	rm -rf $(OBJECTS) wordle.elf wordle.iso
