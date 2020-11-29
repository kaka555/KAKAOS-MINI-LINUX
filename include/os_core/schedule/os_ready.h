#ifndef _OS_READY_H
#define _OS_READY_H

#include <TCB.h>

int _insert_ready_TCB(struct task_struct *TCB_ptr);
int _delete_TCB_from_ready(struct task_struct *TCB_ptr);
struct task_struct *_get_highest_prio_ready_TCB(void);
void shell_check_os_ready(void);

#endif
