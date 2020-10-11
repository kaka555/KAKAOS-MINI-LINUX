#ifndef _DMESG_H
#define _DMESG_H

#include <osbase.h>
#include <myMicroLIB.h>

#define __DMESG __section(".dmseg.data")
#define DMESG_BUFFER_SIZE_KB 1
#define DMESG_BUFFER_SIZE (DMESG_BUFFER_SIZE_KB * 1024)

#ifndef pr_fmt
#define pr_fmt(fmt) fmt
#endif

#define KERN_SOH	"\001"
#define KERN_SOH_ASCII	'\001'

#define KERN_EMERG	KERN_SOH "0"	/* system is unusable */
#define KERN_ALERT	KERN_SOH "1"	/* action must be taken immediately */
#define KERN_CRIT	KERN_SOH "2"	/* critical conditions */
#define KERN_ERR	KERN_SOH "3"	/* error conditions */
#define KERN_WARNING	KERN_SOH "4"	/* warning conditions */
#define KERN_NOTICE	KERN_SOH "5"	/* normal but significant condition */
#define KERN_INFO	KERN_SOH "6"	/* informational */
#define KERN_DEBUG	KERN_SOH "7"	/* debug-level messages */

#define KERN_DEFAULT_LEVEL	4
#define KERN_SHELL	KERN_EMERG
#define KERN_BOOT	KERN_EMERG
#define KERN_SHELL_LEVEL	0

#define KERN_DEFAULT	KERN_SOH "d"	/* the default kernel loglevel */

#define pr_emerg(fmt, ...) \
	ka_printf(KERN_EMERG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_alert(fmt, ...) \
	ka_printf(KERN_ALERT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_crit(fmt, ...) \
	ka_printf(KERN_CRIT pr_fmt(fmt), ##__VA_ARGS__)
#define pr_err(fmt, ...) \
	ka_printf(KERN_ERR pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warning(fmt, ...) \
	ka_printf(KERN_WARNING pr_fmt(fmt), ##__VA_ARGS__)
#define pr_warn pr_warning
#define pr_notice(fmt, ...) \
	ka_printf(KERN_NOTICE pr_fmt(fmt), ##__VA_ARGS__)
#define pr_info(fmt, ...) \
	ka_printf(KERN_INFO pr_fmt(fmt), ##__VA_ARGS__)
#define pr_debug(fmt, ...) \
	ka_printf(KERN_DEBUG pr_fmt(fmt), ##__VA_ARGS__)
#define pr_shell(fmt, ...) \
	ka_printf(KERN_SHELL pr_fmt(fmt), ##__VA_ARGS__)
#define pr_boot(fmt, ...) \
	ka_printf(KERN_BOOT pr_fmt(fmt), ##__VA_ARGS__)

void ka_putchar(char ch, int level);
void shell_dmesg(int argc, char const *argv[]);

static inline int printk_get_level(const char **text)
{
	const char *buffer = *text;
	if (buffer[0] == KERN_SOH_ASCII && buffer[1]) {
		switch (buffer[1]) {
		case '0' ... '7':
			*text = (*text) + 2;
			return c2i(buffer[1]);
		}
	}
	return KERN_DEFAULT_LEVEL;
}

#endif