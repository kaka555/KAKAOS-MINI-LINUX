#include <osbase.h>
#include <myMicroLIB.h>
#include <linker.h>
#include <dmesg.h>
#include <TCB.h>

extern volatile TCB *OSTCBCurPtr;

/* check the disassembly instruction is 'BL' or 'BLX' */
static bool disassembly_ins_is_bl_blx(UINT32 addr) {
	UINT16 ins1 = *((UINT16 *)addr);
	UINT16 ins2 = *((UINT16 *)(addr + 2));

#define BL_INS_MASK         0xF800
#define BL_INS_HIGH         0xF800
#define BL_INS_LOW          0xF000
#define BLX_INX_MASK        0xFF00
#define BLX_INX             0x4700

	if ((ins2 & BL_INS_MASK) == BL_INS_HIGH && (ins1 & BL_INS_MASK) == BL_INS_LOW) {
		return true;
	} else if ((ins2 & BLX_INX_MASK) == BLX_INX) {
		return true;
	} else {
		return false;
	}
}

size_t backtrace_call_stack(UINT32 stack_start_addr, UINT32 stack_size, UINT32 sp)
{
	unsigned int pc;
	unsigned int depth = 0;
	pr_shell("Call Trace:\n");
#define KA_CALL_STACK_MAX_DEPTH 15
	/* copy called function address */
	for (; sp < stack_start_addr + stack_size; sp += sizeof(size_t)) {
		/* the *sp value may be LR, so need decrease a word to PC */
		pc = *((UINT32 *) sp) - sizeof(size_t);
		/* the Cortex-M using thumb instruction, so the pc must be an odd number */
		if (pc % 2 == 0) {
			continue;
		}
		/* fix the PC address in thumb mode */
		pc = *((UINT32 *) sp) - 1;
		if ((pc >= code_start_addr) && (pc <= code_start_addr + code_size) && (depth < KA_CALL_STACK_MAX_DEPTH)
		        /* check the the instruction before PC address is 'BL' or 'BLX' */
		        && disassembly_ins_is_bl_blx(pc - sizeof(size_t))) {
			/* the second depth function may be already saved, so need ignore repeat */
//			if ((depth == 2) && regs_saved_lr_is_valid) {
//				continue;
//			}
			pr_emerg("pc is %x\n", pc);
			depth++;
		}
	}

	return depth;
}

void dump_stack(void)
{
	UINT32 current_sp;
	volatile TCB *current = OSTCBCurPtr;
extern volatile int in_fault;
	if (in_fault)
		asm( "MRS %[result], PSP;"
			: [result]"=r"(current_sp)
		);
	else
		asm( "mov %[result], sp;"
			: [result]"=r"(current_sp)
		);
	backtrace_call_stack((UINT32)current->stack_end, current->stack_size, current_sp);
}