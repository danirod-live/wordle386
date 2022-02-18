/*
 * This file is part of NativeOS
 * Copyright (C) 2015-2021 The NativeOS contributors
 * SPDX-License-Identifier:  GPL-3.0-only
 */

#include <nativeos/i386/include/multiboot.h>
#include <nativeos/include/device.h>
#include <nativeos/include/stdkern.h>
#include <nativeos/include/vfs.h>

/**
 * \file kern/kern_main.c
 * \brief Kernel main entrypoint
 *
 * This function contains code executed right after the platform loader has
 * finished with the platform-dependent stuff. Code in this module should
 * finish with the system initialisation and delegate to the init process
 * to have a proper userland switch. (Or until this is programmed, at least
 * do something useful).
 */

static void wordle_main(void);

/**
 * \brief Kernel main
 *
 * Virtually the main function, although you should know that the platform
 * loader may have done additional stuff before reaching here. Check out
 * code preceding a call to kernel_main in the loader code for each platform.
 */
void
kernel_main(void)
{
	vfs_init();
	device_init();
	wordle_main();
}

static vfs_node_t *
fs_resolve_and_open(const char *path, unsigned int args)
{
	vfs_node_t *node = fs_resolve(path);
	if (node) {
		if (fs_open(node, args) != 0) {
			node = NULL;
		}
	}
	return node;
}

extern void wordle(vfs_node_t *screen, vfs_node_t *kbd, vfs_node_t *clock);

static void
wordle_main(void)
{
	vfs_node_t *screen = fs_resolve_and_open("DEV:/fb", VO_FWRITE);
	vfs_node_t *keyboard = fs_resolve_and_open("DEV:/kbd", VO_FREAD);
	vfs_node_t *clock = fs_resolve_and_open("DEV:/clock", VO_FREAD);
	wordle(screen, keyboard, clock);
}
