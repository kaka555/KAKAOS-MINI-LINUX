#include <myassert.h>
#include <myMicroLIB.h>
#include <dmesg.h>
#include <printf_debug.h>

void my_abort(void)
{
	panic("os stop\n");
	while (1);
}

void _ASSERT(char* strFile, unsigned uLine)
{
	pr_emerg("\nAssertion failed: %s,line %u\r\n", strFile, uLine);
	my_abort();
}
