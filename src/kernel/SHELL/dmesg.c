#include <dmesg.h>
#include <myMicroLIB.h>
#include <buddy.h>
#include <myassert.h>
#include <proc.h>
#include <printf_debug.h>
#include <console.h>

static char __DMESG dmesg_buffer[DMESG_BUFFER_SIZE];
static unsigned long head = 0;;
static unsigned long tail = 0;;

static int current_level = KERN_DEFAULT_LEVEL;

void ka_putchar(char ch, int level)
{
	if (likely(level != KERN_PROMPT_LEVEL)) {
		if ((head + 1) % DMESG_BUFFER_SIZE == tail)
			tail = (tail + 1) % DMESG_BUFFER_SIZE;
		dmesg_buffer[head] = ch;
		head = (head + 1) % DMESG_BUFFER_SIZE;
	}
	if (level < current_level)
		console_putchar(ch);
}

static int dmesg_level_open(struct file *file_ptr)
{
	pr_shell("%d\n", current_level);
	return 0;
}

static struct file_operations proc_dmesg_level_fops = {
	.open = dmesg_level_open,
};

void __INIT dmesg_init(void)
{
	ASSERT(!(DMESG_BUFFER_SIZE & (PAGE_SIZE_BYTE - 1)), "DMESG_SIZE uncorrect\n");
	if (IS_ERR(proc_creat(NULL, "dmesg_level", &proc_dmesg_level_fops))) {
		panic("proc dmesg_level register fail\n");
	}
}
INIT_FUN(dmesg_init, 3);

void shell_dmesg(int argc, char const *argv[])
{
	unsigned long index = tail;
	if (argc > 2) {
		pr_shell("too many arguments!\n");
		return ;
	}
	while (index != head) {
		console_putchar(dmesg_buffer[index]);
		index = (index + 1) % PAGE_SIZE_BYTE;
	}
	if ((2 == argc) && (0 == ka_strcmp(argv[1], "-c")))
		tail = head;
}