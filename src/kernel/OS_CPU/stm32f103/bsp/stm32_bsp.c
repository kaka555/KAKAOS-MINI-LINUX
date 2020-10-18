#include <bsp.h>
#include <buddy.h>
#include <bsp_support.h>
#include <myMicroLIB.h>
#include <platform.h>
#include <console.h>
#include <sys_init_fun.h>
#include <printf_debug.h>

static const struct dev_mem_para para1 = {
	.start = (void *)0x20000000,
	.size = 64, /* KB*/
	.prio = 1,
	.type = TYPE_SYS,
};
/*
static const struct dev_mem_para para2 = {
	.start = (void *)0x68000000,
	.size = 1024, //KB
	.prio = 2,
	.type = TYPE_NORMAL,
};
*/
static const struct bsp_device device_array[] = {
	/*
	{
		.head.dev_name = "USART1",
		.head.dev_info = NULL,
		.head.type = DEV_NORMAL,
		.u.normal.init_fun = USART_Config,
	},
	*/
	/*
		{
			.head.dev_name = "SRAM",
			.head.dev_info = NULL,
			.head.type = DEV_NORMAL,
			.u.normal.init_fun = FSMC_SRAM_Init,
		},
	*/
	/*
		{
			.head.dev_name = "USART2",
			.head.dev_info = NULL,
			.head.type = DEV_NORMAL,
			.u.normal.init_fun = USART2_Config,
		},
	*/
	{
		.head.dev_name = "first mem",
		.head.dev_info = NULL,
		.head.type = DEV_MEM,
		.u.mem.init_fun = __buddy_init,
		.u.mem.para = &para1,
	},
	/*
		{
			.head.dev_name = "second mem",
			.head.dev_info = NULL,
			.head.type = DEV_MEM,
			.u.mem.init_fun = __buddy_init,
			.u.mem.para = &para2,
		}
	*/
};

void bsp_putchar(char ch)
{
	USART_SendData(DEBUG_USARTx, ch);
	while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET);
}

static int _uart1_write(void *data, unsigned int len)
{
	int i = 0;
	char *c_ptr = data;
	while (i < len) {
		bsp_putchar(c_ptr[i]);
		++i;
	}
	return 0;
}

static int _uart1_open(struct device *dev_ptr)
{
	(void)dev_ptr;
	USART_Config();
	return 0;
}

void early_bsp_init(void)
{
	set_early_console_open(_uart1_open);
	set_early_console_write(_uart1_write);
	early_console_init();
	_bsp_init(device_array, sizeof(device_array) / sizeof(device_array[0]));
}

static int uart1_write(struct file *file_ptr, void *buffer, unsigned int len, unsigned int offset)
{
	_uart1_write(buffer, len);
	return len;
}

static struct file_operations uart1_fops = {
	.write = uart1_write,
};

static int uart1_probe(struct device *dev)
{
	return _uart1_open(NULL);
}

static struct platform_driver uart1 = {
	.driver.fops = &uart1_fops,
	.driver.probe = uart1_probe,
};

static void __INIT uart1_driver_register(void)
{
	int ret = platform_driver_register(&uart1, "uart1");
	if (ret)
		panic("uart1 register fail\n");
}
//INIT_FUN(uart1_driver_register, DRIVER_INIT_LEVEL);
DRIVER_INIT_FUN(uart1_driver_register);
