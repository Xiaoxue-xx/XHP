#include "cpu/exec/helper.h"

#define DATA_BYTE 1
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 2
#include "push-template.h"
#undef DATA_BYTE

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

/* for instruction encoding overloading */

make_helper_v(push_r)
//make_helper_v(push_i)
make_helper_v(push_rm)

/*make_helper(pushal) {
	swaddr_write(cpu.esp -  4, 4, cpu.eax, R_SS);
	swaddr_write(cpu.esp -  8, 4, cpu.ecx, R_SS);
	swaddr_write(cpu.esp - 12, 4, cpu.edx, R_SS);
	swaddr_write(cpu.esp - 16, 4, cpu.ebx, R_SS);
	swaddr_write(cpu.esp - 20, 4, cpu.esp, R_SS);
	swaddr_write(cpu.esp - 24, 4, cpu.ebp, R_SS);
	swaddr_write(cpu.esp - 28, 4, cpu.esi, R_SS);
	swaddr_write(cpu.esp - 32, 4, cpu.edi, R_SS);
	cpu.esp -= 32;

	print_asm("pushal");

	return 1;
}*/
