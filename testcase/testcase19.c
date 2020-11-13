/* test exec() */
#include <user.h>
#include <osinit.h>
#include <os_delay.h>
#include <os_cpu.h>

extern volatile TCB *OSTCBCurPtr;
extern volatile TCB *OSTCBHighRdyPtr;

#include <printf_debug.h>

void fun3(void)
{
	UINT32 current_sp;
	pr_shell("now come into function three()\n");
	TCB *current = OSTCBCurPtr;
	asm( "mov %[result], sp;"
	     : [result]"=r"(current_sp)
	   );
	size_t deap = backtrace_call_stack((UINT32)current->stack_end, current->stack_size, current_sp);
	pr_shell("deap is %u\n", deap);
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
