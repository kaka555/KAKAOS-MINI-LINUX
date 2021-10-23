#ifndef _USER_INTERRUPT_H
#define _USER_INTERRUPT_H

#include <TCB.h>

void user_interrupt_register(struct task_struct *task_ptr, user_interrupt_fun fun);

#endif
