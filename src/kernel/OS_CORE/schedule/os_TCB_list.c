#include <os_TCB_list.h>
#include <TCB.h>
#include <kakaosstdint.h>
#include <myassert.h>
#include <myMicroLIB.h>
#include <sys_init_fun.h>

/*
use array to mark struct task_struct with different priority
 */

static struct TCB_list TCB_list[PRIO_MAX];

struct task_struct *find_TCB_with_name(const char *name)
{
	struct task_struct *TCB_ptr;
	int i;
	for (i = 0; i < PRIO_MAX; ++i) {
		list_for_each_entry(TCB_ptr, &TCB_list[i].head, same_prio_list) {
			if (0 == ka_strcmp(TCB_ptr->name, name)) {
				return TCB_ptr;
			}
		}
	}
	return NULL;
}

static void __INIT __init_TCB_list(void)
{
	int i;
	for (i = 0; i < PRIO_MAX; ++i)
	{
		INIT_LIST_HEAD(&TCB_list[i].head);
		TCB_list[i].ready_num = 0;
		TCB_list[i].TCB_num = 0;
	}
}
INIT_FUN(__init_TCB_list, 1);

/*os must set the task_state before using this function*/
void _register_in_TCB_list(struct task_struct *TCB_ptr)
{
	ASSERT(NULL != TCB_ptr, ASSERT_INPUT);
	list_add(&TCB_ptr->same_prio_list, &TCB_list[TCB_ptr->prio].head);
	++(TCB_list[TCB_ptr->prio].TCB_num);
}

/**
 * This is a system function,if the TCB_ptr state is ready,os should decrease
 * the ready num with function _decrease_ready_num() in another place
 * which means, this functin do not decrease the ready num
 * @Author      kaka
 * @DateTime    2018-10-01
 * @description :
 * @param       TCB_ptr    [description]
 * @return                 [description]
 */
int _delete_from_TCB_list(struct task_struct *TCB_ptr)
{
	ASSERT(NULL != TCB_ptr, ASSERT_INPUT);
	struct list_head *pos;
	list_for_each(pos, &TCB_list[TCB_ptr->prio].head)
	{
		if (&TCB_ptr->same_prio_list == pos)
		{
			list_del(pos);
			ASSERT(0 != TCB_list[TCB_ptr->prio].TCB_num, ASSERT_PARA_AFFIRM);
			--(TCB_list[TCB_ptr->prio].TCB_num);
			return 0;
		}
	}
	/*should not go here*/
	ASSERT(0, ASSERT_BAD_EXE_LOCATION);
	return ERROR_USELESS_INPUT;
}

struct list_head *_get_from_TCB_list(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	return &TCB_list[index].head;
}

unsigned char _get_ready_num_from_TCB_list(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	return TCB_list[index].ready_num;
}

void _decrease_ready_num(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	ASSERT(0 != TCB_list[index].ready_num, ASSERT_PARA_AFFIRM);
	--(TCB_list[index].ready_num);
}

void _increase_ready_num(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	++(TCB_list[index].ready_num);
}

void _decrease_TCB_num(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	ASSERT(0 != TCB_list[index].TCB_num, ASSERT_INPUT);
	--(TCB_list[index].TCB_num);
}

void _increase_TCB_num(unsigned int index)
{
	ASSERT(index < PRIO_MAX, ASSERT_INPUT);
	++(TCB_list[index].TCB_num);
}

#if CONFIG_SHELL_EN
void shell_check_TCB_list(void)
{
	unsigned int i;
	unsigned int ready_num = 0, TCB_num = 0;
	struct list_head *pos;
	struct task_struct *TCB_ptr;
	for (i = 0; i < PRIO_MAX; ++i)
	{
		if (!list_empty(&TCB_list[i].head))
		{
			pr_shell("prio %u has task:\n", i);
			list_for_each(pos, &TCB_list[i].head)
			{
				TCB_ptr = list_entry(pos, TCB, same_prio_list);
				++TCB_num;
				if (STATE_READY == TCB_ptr->task_state)
				{
					++ready_num;
				}
				pr_shell("name: %s\n", TCB_ptr->name);
				pr_shell("timeslice_hope_time: %d\n", TCB_ptr->timeslice_hope_time);
				pr_shell("stack: 0x%p\n", (void *)TCB_ptr->stack);
				pr_shell("stack_size: %d\n", TCB_ptr->stack_size);
				pr_shell("stack-base: 0x%p\n", (void *)TCB_ptr->stack_end);
				pr_shell("task_state: ");
				switch (TCB_ptr->task_state)
				{
				case STATE_READY:
					pr_shell("STATE_READY\n"); break;
				case STATE_DELAY:
				case STATE_WAIT_MCB_TIMEOUT:
				case STATE_WAIT_MESSAGE_QUEUE_TIMEOUT:
				case STATE_PUT_MESSAGE_QUEUE_TIMEOUT:
				case STATE_WAIT_MEM_POOL_TIMEOUT:
					pr_shell("STATE_DELAY\n");
					pr_shell("delay reach time is %lu\n", (unsigned long)TCB_ptr->delay_reach_time);
					break;
				case STATE_SUSPEND_NORMAL:
				case STATE_WAIT_MCB_FOREVER:
				case STATE_WAIT_MESSAGE_QUEUE_FOREVER:
				case STATE_PUT_MESSAGE_QUEUE_FOREVER:
				case STATE_WAIT_MUTEX_FOREVER:
					pr_shell("STATE_SUSPEND\n"); break;
				}
				pr_shell("/*****************************************************/\n");
			}
#if CONFIG_DEBUG_ON
			if (TCB_num != TCB_list[i].TCB_num)
			{
				pr_shell("prio %u TCB_num error!\n", i);
			}
			if (ready_num != TCB_list[i].ready_num)
			{
				pr_shell("prio %u ready_num error!\n", i);
			}
#endif
			ready_num = 0;
			TCB_num = 0;
		}
	}
}
#endif

#if CONFIG_SHELL_EN
void shell_stack_check(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;
	unsigned int i;
	struct list_head *pos;
	struct task_struct *TCB_ptr;
	for (i = 0; i < PRIO_MAX; ++i)
	{
		if (!list_empty(&TCB_list[i].head))
		{
			list_for_each(pos, &TCB_list[i].head)
			{
				unsigned int num = 0;
				TCB_ptr = list_entry(pos, TCB, same_prio_list);
				unsigned int *ptr = (unsigned int *)(TCB_ptr->stack_end);
				pr_shell("=====================================================\n");
				pr_shell("task name : %s\n", TCB_ptr->name);
				pr_shell("The stack space is from 0x%p to 0x%p\n", TCB_ptr->stack_end, (STACK_TYPE *)TCB_ptr->stack_end + TCB_ptr->stack_size / 4);
#if CONFIG_DEBUG_ON
				if (0 != *(unsigned int *)(TCB_ptr->stack_end))
				{
					pr_shell("stack full!!!!\n");
					ASSERT(0, ASSERT_BAD_EXE_LOCATION);
				}
#endif
				while (0 == *ptr)
				{
					++num;
					++ptr;
					if ((char *)ptr == (char *)(TCB_ptr->stack_end) + TCB_ptr->stack_size)
					{
						pr_shell("stack empty!!!!\n");
						break;
					}
				}
				pr_shell("It's stack used rate is %d%%\n", 100 - 400 * num / TCB_ptr->stack_size);
			}
		}
	}
}
#endif

#if CONFIG_SHELL_EN

void shell_show_tasks_registers(int argc, char const *argv[])
{
	(void)argc;
	(void)argv;
	unsigned int i;
	struct task_struct *TCB_ptr;
	for (i = 0; i < PRIO_MAX; ++i)
	{
		if (!list_empty(&TCB_list[i].head))
		{
			list_for_each_entry(TCB_ptr, &TCB_list[i].head, same_prio_list)
			{
				const UINT32 *reg = (unsigned int *)(TCB_ptr->stack);
				pr_shell("=====================================================\n");
				pr_shell("task name : %s\n", TCB_ptr->name);
				if (0 == ka_strcmp(TCB_ptr->name, "shell"))
				{
					pr_shell("now show task shell's register is volatile\n");
					continue;
				}
				pr_shell("R4 	= 	0x%x\n", *reg++);
				pr_shell("R5 	= 	0x%x\n", *reg++);
				pr_shell("R6 	= 	0x%x\n", *reg++);
				pr_shell("R7 	= 	0x%x\n", *reg++);
				pr_shell("R8 	= 	0x%x\n", *reg++);
				pr_shell("R9 	= 	0x%x\n", *reg++);
				pr_shell("R10 	= 	0x%x\n", *reg++);
				pr_shell("R11 	= 	0x%x\n", *reg++);
				pr_shell("xPSR	=	0x%x\n", *reg++);
				pr_shell("PC 	= 	0x%x\n", *reg++);
				pr_shell("R12 	= 	0x%x\n", *reg++);
				pr_shell("R3 	= 	0x%x\n", *reg++);
				pr_shell("R2 	= 	0x%x\n", *reg++);
				pr_shell("R1 	= 	0x%x\n", *reg++);
				pr_shell("R0 	= 	0x%x\n", *reg);
				pr_shell("=====================================================\n");
			}
		}
	}
}

#endif
