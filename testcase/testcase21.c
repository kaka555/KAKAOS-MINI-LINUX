/* test user interupt top */
#include <user_interrupt.h>
#include <user.h>
#include <osinit.h>
#include <os_delay.h>
#include <os_cpu.h>
#include <myassert.h>

extern volatile struct task_struct *OSTCBCurPtr;
extern volatile struct task_struct *OSTCBHighRdyPtr;

static struct task_struct *task_three = NULL;

#include <printf_debug.h>

void three(void *para)
{
	pr_shell("this is three\n");
	task_three = (struct task_struct *)OSTCBCurPtr;
	while (1) {
		sleep(2 * HZ);
		ASSERT(OSTCBCurPtr->fun == para, ASSERT_PARA_AFFIRM);
		pr_shell("this is three next\n");
	}
}

void my_fun(void)
{
	pr_shell("user interrupt from task four\n");
	pr_shell("this is task %s\n", OSTCBCurPtr->name);
}

void four(void *para)
{
	pr_shell("this is four\n");
	if (task_three == NULL) {
		pr_shell("user interupt test fail\n");
		return;
	}
	user_interrupt_register(task_three, my_fun);
	pr_shell("register_complete\n");
}
