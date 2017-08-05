/* ------------------------------------------------------------------------ *\
**
**  This file is part of the Chaos Kernel, and is made available under
**  the terms of the GNU General Public License version 2.
**
**  Copyright (C) 2017 - Benjamin Grange <benjamin.grange@epitech.eu>
**
\* ------------------------------------------------------------------------ */

#ifndef _CHAOS_DEBUG_H_
# define _CHAOS_DEBUG_H_

# ifndef _CHAOS_DEF_H_
#  error "You shouldn't include 'debug.h' directly. Include 'chaosdef.h' instead."
# endif /* !_CHAOS_DEF_H_*/

/*
** Blue-screen-like function.
** Prints a message and halts (and catch fire).
*/
void				panic(char const *fmt, ...) __noreturn;

# define static_assert(e)	extern char (*__static_assert(void)) [sizeof(char[1 - 2 * !(e)])]

/*
** The assert() macro will panic if the given expression is evaluated to false
*/
# define			assert(expr)					\
	do {									\
		if (unlikely(!(expr))) {					\
			panic("assert(%s) failed (in %s at line %u).\n",	\
				#expr, __FILE__, __LINE__);			\
		}								\
	}									\
	while (0)

/*
** The assert_eq() macro will panic if the given operands ARE NOT equal.
*/
# define assert_eq(a, b)	assert((a) == (b))

/*
** The assert_neq() macro will panic if the given operands ARE equal.
*/
# define assert_neq(a, b)	assert((a) != (b))

/*
** The assert_lo() macro will panic if the first operand is greater than the other.
*/
# define assert_lo(a, b)	assert((a) < (b))

/*
** The assert_gr() macro will panic if the first operand is lower than the other.
*/
# define assert_gr(a, b)	assert((a) > (b))

#endif /* !_CHAOS_DEBUG_H_ */
