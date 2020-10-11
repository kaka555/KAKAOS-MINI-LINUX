#include <console.h>
#include <os_error.h>
#include <printf_debug.h>
#include <cmdline.h>

static struct console *current_console = NULL;

static void console_setup(const char *str)
{
	if (current_console)
		panic("console has been registered\n");
}
_setup("console=", console_setup);
