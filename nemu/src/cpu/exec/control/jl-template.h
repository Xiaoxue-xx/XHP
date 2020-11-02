#include "cpu/exec/template-start.h"

#define instr jl

static void do_execute(){
	DATA_TYPE_S displacement = op_src->val;
//	print_asm("jl %x",cpu.eip+1+DATA_BYTE+displacement);
	if(cpu.SF!=cpu.OF){cpu.eip+=displacement;
#if DATA_BYTE ==2
	cpu.eip &=0xffff;
#endif
}
	print_asm("jl %x",cpu.eip+1+DATA_BYTE);
}
make_instr_helper(i)

#include "cpu/exec/template-end.h"

