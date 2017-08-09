/* ------------------------------------------------------------------------ *\
**
**  This file is part of the Chaos Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2017 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\* ------------------------------------------------------------------------ */

#include <kernel/spinlock.h>
#include <kernel/thread.h>
#include <kernel/interrupts.h>
#include <debug.h>

extern struct thread thread_table[MAX_PID];
extern struct thread *init_thread;
extern struct spinlock thread_table_lock;

/*
** Looks for the next runnable thread.
*/
static struct thread *
find_next_thread(void)
{
	bool pass;
	struct thread *limit;
	struct thread *t;

	pass = false;
	t = get_current_thread() + 1;
	limit = thread_table + MAX_PID;

look_for_next:
	while (t < limit)
	{
		if (t->state == RUNNABLE) {
			return (t);
		}
		t++;
	}
	if (!pass)
	{
		t = thread_table;
		limit = get_current_thread() + 1;
		pass = true;
		goto look_for_next;
	}
	return (get_current_thread());
}

/*
** Finds and executes the next runnable thread.
*/
void
thread_reschedule(void)
{
	struct thread *new;
	struct thread *old;

	assert(!are_int_enabled());
	assert(holding_lock(&thread_table_lock));

	old = get_current_thread();
	new = find_next_thread();
	new->state = RUNNING;
	if (new != old)
	{
		set_current_thread(new);

		/* TODO memory space switch */

		arch_context_switch(old, new);
	}
}

/*
** Yield the cpu to an other thread.
** This function will return at a later time, or
** possibly immediately if no other threads are waiting
** to be executed. (don't worry, they are guilty)
*/
void
thread_yield(void)
{
	struct thread *t;

	t = get_current_thread();
	LOCK_THREAD(state);

	assert(t->state == RUNNING);

	t->state = RUNNABLE;
	thread_reschedule();

	RELEASE_THREAD(state);
}

enum handler_return
irq_timer_handler(void)
{
	return (IRQ_RESCHEDULE);
}
