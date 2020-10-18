#include <console.h>
#include <os_error.h>
#include <printf_debug.h>
#include <cmdline.h>
#include <sys_init_fun.h>
#include <shell.h>
#include <platform.h>
#include <myassert.h>

static struct console default_console;
static struct file_operations *console_file_fops;
static struct console *current_console = &default_console;

static int __INIT dealult_console_write(void *data, unsigned int len)
{
	(void)NULL;
	return 0;
}

/*
static int __INIT dealult_console_read(void *data, unsigned int len)
{
	(void)NULL;
	return 0;
}
*/

static struct console default_console = {
	.write = dealult_console_write,
	/*.read = dealult_console_read,*/
};

static int current_console_open(struct device *dev_ptr)
{
	console_file_fops = dev_ptr->driver_ptr->fops;
	if (console_file_fops->open)
		console_file_fops->open(NULL);
	return 0;
}

static int current_console_write(void *data, unsigned int len)
{
	return console_file_fops->write(NULL, data, len, 0);
}

static int current_console_read(void *data, unsigned int len)
{
	if (console_file_fops->read)
		return console_file_fops->read(NULL, data, len, 0);
	return 0;
}

static struct console console = {
	.open = current_console_open,
	.write = current_console_write,
	.read = current_console_read,
};
static char console_name[CONSOLE_NAME_LEN];

void early_console_init(void)
{
	if (default_console.open)
		default_console.open(NULL);
}

int console_read(void *data, unsigned int len)
{
	int ret = current_console->read(data, len);
	int i = 0;
	char *c_ptr = data;
	for (i = 0; i < len; ++i)
		_put_in_shell_buffer(c_ptr[i]);
	return ret;
}

int console_write(void *data, unsigned int len)
{
	return current_console->write(data, len);
}

void set_early_console_open(int (*early_open)(struct device *dev_ptr))
{
	default_console.open = early_open;
}

void set_early_console_read(int (*early_read)(void *data, unsigned int len))
{
	default_console.read = early_read;
}

void set_early_console_write(int (*early_write)(void *data, unsigned int len))
{
	default_console.write = early_write;
}

static struct platform_device console_platform_dev;

static void console_dev_register(void)
{
	int ret = wo_platform_device_register(&console_platform_dev, console_name);
	if (ret)
		panic("console register return %d\n", ret);
	if (!device_has_matched(&console_platform_dev.device))
		panic("console has no driver\n");
	console.dev_ptr = &console_platform_dev.device;
}
DEV_INIT_FUN(console_dev_register);

static void __INIT parse_console(const char *str)
{
	unsigned int len;
	pr_boot("in parse_console");
	if (!str)
		panic("no console set\n");
	const char *ptr = str;
	while (*ptr && (' ' != *ptr))
		++ptr;
	len = ptr - str + 1;
	ASSERT(len < CONSOLE_NAME_LEN - 1, ASSERT_PARA_AFFIRM);
	ka_memcpy(console_name, str, len - 1);
	console_name[len - 1] = '\0';
}

static void __INIT console_setup(const char *str)
{
	if ((current_console) && (current_console != &default_console))
		panic("console has been registered\n");
	parse_console(str);
}
_setup("console=", console_setup);

void console_init(void)
{
	if (default_console.close)
		default_console.close();
	current_console = &console;
	console.open(console.dev_ptr);
}
