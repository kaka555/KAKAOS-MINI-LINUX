#ifndef _OS_ERROR_H
#define _OS_ERROR_H

#include <myMicroLIB.h>

/*when the following #define, add a ‘-’ before it
  example:
  return -ERROR_NULL_INPUT_PTR;*/
#define ERROR_NULL_INPUT_PTR   		1
#define ERROR_USELESS_INPUT  		2
#define ERROR_FUN_USE_IN_INTER 		3
#define ERROR_EXECUTE_IN_INTERRUPT  4
#define ERROR_NO_MEM				5
#define ERROR_MODULE_DELETED		6
#define ERROR_LOGIC					7
#define ERROR_SYS 					8
#define ERROR_DISK					9
#define ERROR_FAULT					10 /* bad address */
#define ERROR_BUSY					11

#define _must_check
#define PUBLIC
#define PRIVATE

#define KA_TRUE		1
#define KA_FALSE	0

#define OS_ERROR_MESSAGE_DISPLAY(text) 			ka_printf(#text)
#define OS_ERROR_PARA_MESSAGE_DISPLAY(fun,para)	ka_printf("error fun name : " #fun "\n""error para : " #para "\n")

#endif
