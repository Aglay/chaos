/* ------------------------------------------------------------------------ *\
**
**  This file is part of the Chaos Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2017 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\* ------------------------------------------------------------------------ */

#include <kernel/init.h>
#include <kernel/pmm.h>
#include <kernel/unit-tests.h>
#include <kernel/multiboot.h>
#include <string.h>
#include <stdio.h>

/*
** This is a pretty naive frame allocator using a bitmap to memorize which
** frames are free and which ones are not.
**
** It can definitely be optimised, but that's not the point at this moment.
*/

static uchar				frame_bitmap[FRAME_BITMAP_SIZE];
static size_t				next_frame;

/*
** Allocates a new frame and returns it, or NULL_FRAME if there is no physical
** memory left.
**
** The idea is that next_frame contains the index in frame_bitmap of our first
** looking address, the one most likely to be free.
** The search is done in two steps, the first one from next_frame to NB_FRAMES, and
** the second one from 0 to next_frame. If after these two steps no
** free frame was found, then NULL_FRAME is returned. In the other case,
** it also sets next_frame to the index of the following address.
*/
phys_addr_t
alloc_frame(void)
{
	size_t i;
	size_t j;
	size_t final;
	bool pass;

	i = next_frame;
	final = FRAME_BITMAP_SIZE;
	pass = false;
look_for_address:
	while (i < final)
	{
		if (frame_bitmap[i] != 0xFFu) {
			j = 0u;
			while (j < 7u && (frame_bitmap[i] & (1 << j)) != 0) {
				++j;
			}
			frame_bitmap[i] |= (1 << j);
			next_frame = i;
			return (PAGE_SIZE * (i * 8u + j));
		}
		++i;
	}
	if (!pass)
	{
		final = next_frame;
		i = 0;
		pass = true;
		goto look_for_address;
	}
	return (NULL_FRAME);
}

/*
** Frees a given frame.
*/
void
free_frame(phys_addr_t frame)
{
	/* Ensure the address is page-aligned */
	assert(IS_PAGE_ALIGNED(frame));

	/* Ensure the given physical address is taken */
	assert(is_frame_allocated(frame));

	/* Set the bit corresponding to this frame to 0 */
	frame_bitmap[GET_FRAME_IDX(frame)] &= ~(GET_FRAME_MASK(frame));
	next_frame = GET_FRAME_IDX(frame);
}

/*
** Returns true if the given address is taken.
*/
bool
is_frame_allocated(phys_addr_t frame)
{
	assert(IS_PAGE_ALIGNED(frame));
	return (frame_bitmap[GET_FRAME_IDX(frame)] & (GET_FRAME_MASK(frame)));
}

/*
** Mark a frame as allocated.
*/
static inline void
mark_frame_as_allocated(phys_addr_t frame)
{
	assert(IS_PAGE_ALIGNED(frame));
	frame_bitmap[GET_FRAME_IDX(frame)] |= GET_FRAME_MASK(frame);
}

/*
** Mark a frame as freed.
*/
static inline void
mark_frame_as_free(phys_addr_t frame)
{
	assert(IS_PAGE_ALIGNED(frame));
	frame_bitmap[GET_FRAME_IDX(frame)] &= ~GET_FRAME_MASK(frame);
}

/*
** Mark a range of frames as allocated.
*/
void
mark_range_as_allocated(phys_addr_t start, phys_addr_t end)
{
	assert(IS_PAGE_ALIGNED(start));
	assert(IS_PAGE_ALIGNED(end));

	while (start <= end)
	{
		mark_frame_as_allocated(start);
		start += PAGE_SIZE;
	}
}

/*
** Mark a range of frames as freed.
*/
void
mark_range_as_free(phys_addr_t start, phys_addr_t end)
{
	assert(IS_PAGE_ALIGNED(start));
	assert(IS_PAGE_ALIGNED(end));

	while (start <= end)
	{
		mark_frame_as_free(start);
		start += PAGE_SIZE;
	}
	next_frame = end;
}

/*
** Reset the Physical Memory Manager.
*/
static void
pmm_reset(void)
{
	multiboot_memory_map_t *mmap;

	next_frame = 0u;

	/* Mark everything as allocated */
	memset(frame_bitmap, 0xFF, sizeof(frame_bitmap));

	/* Parse the multiboot structure to mark memory areas that aren't available */
	mmap = multiboot_infos.mmap;
	while (mmap < multiboot_infos.mmap_end)
	{
		if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
			mark_range_as_free(
				ROUND_DOWN(mmap->addr, PAGE_SIZE),
				ALIGN(mmap->addr + mmap->len, PAGE_SIZE)
			);
		}
		mmap = (multiboot_memory_map_t *)((uchar *)mmap + multiboot_infos.mmap_entry_size);
	}

	/* Mark the kernel as allocated */
	mark_range_as_allocated(0, ALIGN(KERNEL_PHYSICAL_END, PAGE_SIZE));
}

/*
** Initializes the frame allocator.
*/
static void
pmm_init(enum init_level il __unused)
{
	pmm_reset();
	printf("[OK]\tPhysical Memory Managment (Size: %r)\n", (multiboot_infos.mem_stop - multiboot_infos.mem_start) * 1024u);
}

/*
** Some unit tests for the frame allocator.
*/
static void
pmm_test(void)
{
	/* Mark everything as free for the unit tests (will be reversed after) */
	memset(frame_bitmap, 0x00, sizeof(frame_bitmap));

	assert(!is_frame_allocated(0xfffff000));
	mark_frame_as_allocated(0xfffff000);
	assert(is_frame_allocated(0xfffff000));
	free_frame(0xfffff000);
	assert(!is_frame_allocated(0xfffff000));
	next_frame = 0;

	assert(!is_frame_allocated(0x0));
	assert_eq(alloc_frame(), 0x0000);
	assert_eq(alloc_frame(), 0x1000);
	assert_eq(alloc_frame(), 0x2000);
	assert(is_frame_allocated(0x0));
	assert(!is_frame_allocated(0xfffff000));
	free_frame(0x1000);
	assert_eq(alloc_frame(), 0x1000);
	free_frame(0x0000);
	free_frame(0x1000);
	free_frame(0x2000);
	while (alloc_frame() != NULL_FRAME);
	assert_eq(alloc_frame(), NULL_FRAME);
	free_frame(1234 * 0x1000);
	assert_eq(alloc_frame(), 1234 * 0x1000);
	assert_eq(alloc_frame(), NULL_FRAME);
	free_frame(0xfffff000);
	assert_eq(alloc_frame(), 0xfffff000);
	assert_eq(alloc_frame(), NULL_FRAME);
	free_frame(0x0);
	assert_eq(alloc_frame(), 0x0);
	assert_eq(alloc_frame(), NULL_FRAME);
	assert(is_frame_allocated(0x0));
	assert(is_frame_allocated(0xfffff000));

	/* Reset PMM */
	pmm_reset();
}

NEW_INIT_HOOK(pmm, &pmm_init, CHAOS_INIT_LEVEL_PMM);
NEW_UNIT_TEST(pmm, &pmm_test, UNIT_TEST_LEVEL_PMM);
