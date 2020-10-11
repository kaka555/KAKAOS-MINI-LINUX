#include <myassert.h>
#include <myMicroLIB.h>
#include <dmesg.h>

void my_abort(void)
{
	pr_emerg("os stop\n");
	while (1);
}

void _ASSERT(char* strFile, unsigned uLine)
{
	pr_emerg("\nAssertion failed: %s,line %u\r\n", strFile, uLine);
	my_abort();
}
