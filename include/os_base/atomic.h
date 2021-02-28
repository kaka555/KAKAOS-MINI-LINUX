#ifndef _ATOMIC_H
#define _ATOMIC_H

typedef struct atomic_type
{
	int count;
} atomic_t;

static inline void atomic_init(atomic_t *atomic_ptr, int value)
{
	atomic_ptr->count = value;
}

static inline void atomic_add(atomic_t *atomic_ptr)
{
	++atomic_ptr->count;
}

static inline void atomic_dec(atomic_t *atomic_ptr)
{
	if (unlikely(0 == atomic_ptr->count))
		return;
	--atomic_ptr->count;
}

#endif