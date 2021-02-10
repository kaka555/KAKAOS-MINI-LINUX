#ifndef _OS_TCB_LIST_H
#define _OS_TCB_LIST_H

#include <double_linked_list.h>
#include <TCB.h>

struct TCB_list {
	struct list_head head;
	unsigned char ready_num;
	unsigned char TCB_num;
};

struct task_struct *find_TCB_with_name(const char *name);
void _register_in_TCB_list(struct task_struct *TCB_ptr);
int _delete_from_TCB_list(struct task_struct *TCB_ptr);
struct list_head *_get_from_TCB_list(unsigned int index);
unsigned char _get_ready_num_from_TCB_list(unsigned int index);
void _decrease_ready_num(unsigned int index);
void _increase_ready_num(unsigned int index);
void _decrease_TCB_num(unsigned int index);
void _increase_TCB_num(unsigned int index);
void shell_check_TCB_list(void);
void shell_stack_check(int argc, char const *argv[]);
void shell_show_tasks_registers(int argc, char const *argv[]);

extern struct TCB_list TCB_list[PRIO_MAX];

#define for_each_task(i, TCB_ptr) \
	for (i = 0; i < PRIO_MAX; ++i) \
		list_for_each_entry(TCB_ptr, &TCB_list[i].head, same_prio_list)

#define TOP_ARRAY_MAX 10

struct top {
	struct task_struct *task;
	UINT64 base_time;
};

#endif
