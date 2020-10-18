#ifndef _CONSOLE_H
#define _CONSOLE_H

#include <device.h>

#define CONSOLE_NAME_LEN 10

struct console
{
	struct device *dev_ptr;
	int (*open)(struct device *dev_ptr);
	int (*write)(void *data, unsigned int len);
	int (*read)(void *data, unsigned int len);
	void (*close)(void);
};

int console_read(void *data, unsigned int len);
int console_write(void *data, unsigned int len);
void set_early_console_open(int (*early_open)(struct device *dev_ptr));
void set_early_console_read(int (*early_read)(void *data, unsigned int len));
void set_early_console_write(int (*early_write)(void *data, unsigned int len));
void early_console_init(void);
void console_init(void);

static inline void console_putchar(char c)
{
	console_write(&c, 1);
}

#endif
