#ifndef _CMDLINE_H
#define _CMDLINE_H

#include <osbase.h>

struct cmdline_setup
{
	const char *command_name;
	void (*setup_fun)(const char *str);
};

#define _setup(cmd_name, fun) 	\
const struct cmdline_setup __kaka_cmdline_setup##fun __section(".CmdlineSetup") = \
{								\
    .command_name = cmd_name,	\
    .setup_fun = fun,			\
}

/*
	should be used like  _setup("console=", console_parse)
 */

extern unsigned long _ka_setup_fun_begin;
extern unsigned long _ka_setup_fun_end;

#define CMDLINE_SETUP_START (struct cmdline_setup *)&_ka_setup_fun_begin
#define CMDLINE_SETUP_END	(struct cmdline_setup *)&_ka_setup_fun_end

void cmdline_parse(void);

#endif
