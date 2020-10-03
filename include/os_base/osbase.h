#ifndef _BARRIER_H
#define _BARRIER_H

#define barrier() __asm__ __volatile__("" ::: "memory")
#define do_nothing()  NULL
#define __section(S) __attribute__ ((__section__(S)))

typedef int bool;

#define IS_ERR_VALUE(x) ((x) <= (long)0)

static inline void *ERR_PTR(long error)
{
	return (void *) error;
}

static inline long PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static inline bool IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((long)ptr);
}

#endif
