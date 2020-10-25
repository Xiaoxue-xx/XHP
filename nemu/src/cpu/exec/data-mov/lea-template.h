#include "cpu/exec/template-start.h"

#define instr lea

static void do_execute(){
	DATA_TYPE addr = op_src->reg;
	OPERAND_W(op_dest,addr);
	print_asm_template2();
}
make_instr_helper(rm2r)

#include "cpu/exec/template-end.h"
