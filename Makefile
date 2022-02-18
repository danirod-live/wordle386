FILES = support/hardware/kbd.c \
	support/hardware/vgafb.c \
	support/hardware/port.c \
	support/hardware/rtclock.c \
	support/hardware/spinlock.c \
	support/hardware/pctimer.c \
	support/stdkern/strcat.c \
	support/stdkern/strchr.c \
	support/stdkern/ringbuf.c \
	support/stdkern/list.c \
	support/stdkern/strlen.c \
	support/stdkern/memcpy.c \
	support/stdkern/strcmp.c \
	support/stdkern/memset.c \
	support/stdkern/strsep.c \
	support/stdkern/strcpy.c \
	support/stdkern/strdup.c \
	support/vfs/fs_devfs.c \
	support/vfs/fs_fsops.c \
	support/vfs/fs_path.c \
	support/vfs/fs_vfs.c \
	support/i386/multiboot.S \
	support/i386/kernel/cpu/idt.c \
	support/i386/kernel/cpu/lidt.S \
	support/i386/kernel/mem/alloc.c \
	support/i386/kernel/mem/heap.c \
	support/i386/kernel/mem/pmm.c \
	wordleOS/wordle.c \
	wordleOS/kern_main.c \
	wordleOS/locore.S

OBJECTS = support/hardware/kbd.o \
	support/hardware/vgafb.o \
	support/hardware/port.o \
	support/hardware/rtclock.o \
	support/hardware/spinlock.o \
	support/hardware/pctimer.o \
	support/stdkern/strcat.o \
	support/stdkern/strchr.o \
	support/stdkern/ringbuf.o \
	support/stdkern/list.o \
	support/stdkern/strlen.o \
	support/stdkern/memcpy.o \
	support/stdkern/strcmp.o \
	support/stdkern/memset.o \
	support/stdkern/strsep.o \
	support/stdkern/strcpy.o \
	support/stdkern/strdup.o \
	support/vfs/fs_devfs.o \
	support/vfs/fs_fsops.o \
	support/vfs/fs_path.o \
	support/vfs/fs_vfs.o \
	support/i386/multiboot.o \
	support/i386/kernel/cpu/idt.o \
	support/i386/kernel/cpu/lidt.o \
	support/i386/kernel/mem/alloc.o \
	support/i386/kernel/mem/heap.o \
	support/i386/kernel/mem/pmm.o \
	wordleOS/wordle.o \
	wordleOS/kern_main.o \
	wordleOS/locore.o


wordle.elf: $(OBJECTS)
	ld.lld -nostdlib -T wordleOS/kernel.ld $(OBJECTS) -o wordle.elf

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
