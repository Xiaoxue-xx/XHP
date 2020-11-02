#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute(){
	DATA_TYPE_S displacement = op_src->val;
//	print_asm("jbe %x",cpu.eip+1+DATA_BYTE+displacement);
	if(cpu.CF==1||cpu.ZF==1){cpu.eip+=displacement;
#if DATA_BYTE ==2
        cpu.eip&=0xffff;
#endif
}
       print_asm("ja %x",cpu.eip+1+DATA_BYTE);

}
make_instr_helper(i)

#include "cpu/exec/template-end.h"
