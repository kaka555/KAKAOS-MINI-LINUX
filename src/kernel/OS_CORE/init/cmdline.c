#include <cmdline.h>
#include <sys_init_fun.h>
#include <kakaosstdint.h>
#include <myMicroLIB.h>
#include <dmesg.h>
#include <printf_debug.h>

//#define CMDLINE "console=uart1 mem=0x20000000,64k"
#define CMDLINE "console=uart1"

#define for_each_command(start) \
for (start = CMDLINE_SETUP_START; start != CMDLINE_SETUP_END; start += sizeof(struct cmdline_setup))

static void __INIT cmdline_disintegrate(const char **start,
                                        const char **parm, const char **end)
{
	const char *st = *start;
	*parm = NULL;
	while (*st == ' ')
		++st;
	*start = st;
	while (*st) {
		if (*st == '=') {
			*parm = st + 1;
			++st;
			break;
		}
		++st;
	}
	if (NULL == *parm)
		*parm = st; //this is for some cmd without '='
	if (*st == '\0') {
		*end = NULL;
		return ;
	}
	while ((*st != ' ') && (*st))
		++st;
	*end = st;
}

static void __INIT lookup_cmd(const char *cmd, int len, const char *parm)
{
	struct cmdline_setup *cmdline_setup_ptr;
	for_each_command(cmdline_setup_ptr) {
		if (0 == ka_strncmp(cmd, cmdline_setup_ptr->command_name, len)) {
			cmdline_setup_ptr->setup_fun(parm);
			return ;
		}
	}
}

void __INIT cmdline_parse(void)
{
	const char *start = CMDLINE;
	const char *parm;
	const char *end;
	while (1) {
		cmdline_disintegrate(&start, &parm, &end);
		if (NULL == end)
			break;
		lookup_cmd(start, parm - start, parm);
		start = end;
	}
}