#include <user_interrupt.h>
#include <os_cpu_stm32.h>
#include <myassert.h>

extern volatile struct task_struct *OSTCBCurPtr;

#define current OSTCBCurPtr
#define SAVED_REGISTER_NUM 15

static void restore_frame(void)
{
	/* we put some register in stack to build a stack frame for
		user interrupt function, so now we have to restore it*/
	current->stack += SAVED_REGISTER_NUM; 
	OSIntCtxSw();
}

static void default_user_interrupt_handler(void *para)
{
	ASSERT(NULL == para, ASSERT_PARA_AFFIRM);
	struct task_struct *task_ptr = (struct task_struct *)current;
	user_interrupt_fun fun = task_ptr->fun;
	fun();
	restore_frame();
}

static inline void user_interrupt_set_task_frame(struct task_struct *task_ptr)
{
	set_register((void **)&task_ptr->stack, default_user_interrupt_handler, NULL, NULL);
}

void user_interrupt_register(struct task_struct *task_ptr, user_interrupt_fun fun)
{
	ASSERT(task_ptr != current, ASSERT_PARA_AFFIRM);
	ASSERT(fun != NULL, ASSERT_PARA_AFFIRM);
	task_ptr->fun = fun;
	user_interrupt_set_task_frame(task_ptr);
	return;
}

