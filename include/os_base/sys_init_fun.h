#ifndef _SYS_INIT_FUN_H
#define _SYS_INIT_FUN_H

#include <osbase.h>

#define __INIT  __section(".INIT.TEXT")

#define __RAMFUNC __attribute__ ((long_call, section (".ramfunctions")))

typedef void (*init_fun_type)(void);

struct init_fun
{
	init_fun_type fun;
};

#define _INIT_FUN(function,level)	\
const struct init_fun __init_fun_##function __section(".INIT_FUN" #level) = \
{	\
	.fun = function,\
}

#define INIT_FUN(function,level) _INIT_FUN(function,level)

#define DRIVER_INIT_LEVEL 4
#define DRIVER_INIT_FUN(function)	\
	INIT_FUN(function, DRIVER_INIT_LEVEL)

#define DEV_INIT_LEVEL 5
#define DEV_INIT_FUN(function)	\
	INIT_FUN(function, DEV_INIT_LEVEL)

#endif
