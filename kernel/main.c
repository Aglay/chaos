/* ------------------------------------------------------------------------ *\
**
**  This file is part of the Chaos Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2017 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\* ------------------------------------------------------------------------ */

#include <kernel/init.h>
#include <kernel/thread.h>
#include <kernel/options.h>
#include <kernel/multiboot.h>
#include <kernel/alloc.h>
#include <lib/interrupts.h>
#include <stdio.h>
#include <string.h>

static void dumper(void)
{
	assert(are_int_enabled());
	thread_dump();
}

static void init(void)
{
	size_t i;
	struct thread *t;
	void *ptr;

	i = 500000;
	while (42) {
		// Test allocations and interrupts
		while(++i < 500000) {
			assert(are_int_enabled());
		}
		disable_interrupts();
		ptr = kalloc(i / 100);
		enable_interrupts();
		while(++i < 1000000) {
			assert(are_int_enabled());
		}
		disable_interrupts();
		kfree(ptr);
		enable_interrupts();

		// Dump threads
		t = thread_create("dumper", &dumper, DEFAULT_STACK_SIZE);
		assert_neq(t, NULL);
		thread_resume(t);
		i = 0;
	}
}

/*
** Common entry point of the kernel.
*/
int
kernel_main(uintptr mb_addr)
{
	struct thread *t;

	/* Put us in the boot thread */
	thread_init();

	/* Super super early hooks goes first. */
	kernel_init_level(CHAOS_INIT_LEVEL_EARLIEST, CHAOS_INIT_LEVEL_ARCH_EARLY - 1);

	/* Then goes early arch and platform stuff */
	kernel_init_level(CHAOS_INIT_LEVEL_ARCH_EARLY, CHAOS_INIT_LEVEL_PLATFORM_EARLY - 1);
	kernel_init_level(CHAOS_INIT_LEVEL_PLATFORM_EARLY, CHAOS_INIT_LEVEL_PMM - 1);

	/* Load the multiboot structure */
	multiboot_load(mb_addr);

	/* Initialize the memory management */
	kernel_init_level(CHAOS_INIT_LEVEL_PMM, CHAOS_INIT_LEVEL_VMM - 1);
	kernel_init_level(CHAOS_INIT_LEVEL_VMM, CHAOS_INIT_LEVEL_ARCH - 1);

	/* It's time to initialize arch, platform and drivers */
	kernel_init_level(CHAOS_INIT_LEVEL_ARCH, CHAOS_INIT_LEVEL_PLATFORM - 1);
	kernel_init_level(CHAOS_INIT_LEVEL_PLATFORM, CHAOS_INIT_LEVEL_LATEST);

	t = thread_create("init", &init, DEFAULT_STACK_SIZE);
	thread_resume(t);

	/* Print hello message */
	printf("\nWelcome to ChaOS\n\n");

	/* Become the idle thread and enable interrupts */
	thread_become_idle();

	return (0);
}
