#ifndef _LINKER_H
#define _LINKER_H

extern unsigned long _stext;
extern unsigned long _etext;

#define code_start_addr ((unsigned long)&_stext)
#define code_end_addr	((unsigned long)&_etext)
#define code_size		(code_end_addr - code_start_addr)

#endif