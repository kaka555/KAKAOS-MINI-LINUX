/* test command top */
#include <user.h>
#include <osinit.h>
#include <os_delay.h>
#include <os_cpu.h>

extern volatile struct task_struct *OSTCBCurPtr;
extern volatile struct task_struct *OSTCBHighRdyPtr;

#include <printf_debug.h>

void __attribute__((optimize("O0"))) three(void *para)
{
	pr_shell("this is three\n");
	while (1) {
		sleep(2 * HZ);
		unsigned int num = 1600000;
		while (num--);
		ka_printf("this is three\n");
	}
}

void __attribute__((optimize("O0"))) four(void *para)
{
	pr_shell("this is four\n");
	while (1) {
		sleep(HZ);
		unsigned int num = 1000000;
		while (num--);
		ka_printf("this is four\n");
	}
}
