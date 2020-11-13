#ifndef _BARRIER_H
#define _BARRIER_H

#define barrier() __asm__ __volatile__("" ::: "memory")
#define do_nothing()  NULL
#define __section(S) __attribute__ ((__section__(S)))

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define likely(x)	__builtin_expect(!!(x), 1)
#define unlikely(x)	__builtin_expect(!!(x), 0)

typedef int bool;
#define true		1
#define false		0

#define IS_ERR_VALUE(x) ((x) <= (long)0)

static inline void *ERR_PTR(long error)
{
	return (void *)error;
}

static inline long PTR_ERR(const void *ptr)
{
	return (long)ptr;
}

static inline bool IS_ERR(const void *ptr)
{
	return unlikely(IS_ERR_VALUE((long)ptr));
}

static inline char i2c(int num)
{
	return (num + ('1' - 1));
}

static inline int c2i(char c)
{
	return (c + (1 - '1'));
}

#endif
