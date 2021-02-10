/* test exec() */
#include <user.h>
#include <osinit.h>
#include <os_delay.h>
#include <os_cpu.h>

extern volatile struct task_struct *OSTCBCurPtr;
extern volatile struct task_struct *OSTCBHighRdyPtr;

#include <printf_debug.h>

void fun3(void)
{
	UINT32 current_sp;
	pr_shell("now come into function three()\n");
	struct task_struct *current = OSTCBCurPtr;
	asm( "mov %[result], sp;"
	     : [result]"=r"(current_sp)
	   );
	size_t deap = backtrace_call_stack((UINT32)current->stack_end, current->stack_size, current_sp);
	pr_shell("deap is %u\n", deap);
	//pr_shell("now going to trigger panic\n");
	//*(int *)0 = 0;
	//while(1) ;
}

void fun2(void)
{
	fun3();
}

void fun1(void)
{
	fun2();
}

void __attribute__((optimize("O0"))) three(void *para)
{
	fun1();
}
