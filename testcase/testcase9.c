//test context switch's time
#include <user.h>
#include <osinit.h>
#include <os_delay.h>

extern volatile struct task_struct *OSTCBCurPtr;
extern volatile struct task_struct *OSTCBHighRdyPtr;

#include "bsp_led.h"
void three(void *para)
{
	LED_GPIO_Config();
	LED1_OFF;
	sleep(100);
	//suspend();
	while (1);
}

void four(void *para)
{
	LED1_ON;
	while (1);
}
//void five(void *para)
//{
//	while(1);
//}
//void six(void *para)
//{
//	while(1);
//}
