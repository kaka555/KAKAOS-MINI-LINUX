#ifndef _PRINTF_DEBUG_DEBUG
#define _PRINTF_DEBUG_DEBUG
#include <ka_configuration.h>
#include <dmesg.h>
#include <os_cpu.h>

#if CONFIG_PRINTF_DEBUG
#define PRINTF(format, ...) ka_printf (format, ##__VA_ARGS__)
#else
#define PRINTF(format, ...)
#endif

#if CONFIG_PRINTF_DEBUG
#define KA_WARN(type, format, ...)                                       \
	do                                                                            \
	{                                                                             \
	    if (type)                                                                 \
	        pr_info(format, ##__VA_ARGS__);                                   \
	}                                                                             \
	while(0)

#define DEBUG_TYPE_VFS 			0
#define DEBUG_TYPE_SHELL_TAB 	0
#define DEBUG_TYPE_MODULE 		1
#define DEBUG_TYPE_VECTOR 		1
#define DEBUG_TYPE_SHELL 		0
#define DEBUG_TYPE_MALLOC		1
#define DEBUG_FAT				1
#define DEBUG_MEM_POOL			1
#define DEBUG_EXEC              1
#else
#define KA_WARN(type, format, ...)
#endif

#define panic(format, ...) \
		do{ \
			pr_emerg("file_name: %s\tline_num: %u\tfunction_name: %s\n", \
				__FILE__,__LINE__,__FUNCTION__); \
			pr_emerg("panic::" format,##__VA_ARGS__);\
			dump_stack(); \
			while(1);\
		}while(0)

#define MARK() pr_emerg("file_name: %s\tline_num: %u\tfunction_name: %s\n", \
				__FILE__,__LINE__,__FUNCTION__);

#endif
