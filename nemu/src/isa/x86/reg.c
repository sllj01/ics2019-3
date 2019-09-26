#include "nemu.h"
#include <stdlib.h>
#include <time.h>
#include <inttypes.h>
#include <string.h>
const char *regsl[] = {"eax", "ecx", "edx", "ebx", "esp", "ebp", "esi", "edi"};
const char *regsw[] = {"ax", "cx", "dx", "bx", "sp", "bp", "si", "di"};
const char *regsb[] = {"al", "cl", "dl", "bl", "ah", "ch", "dh", "bh"};
/*char* strlwr(const char* ps)
{	char product
	while (*ps){
		if('A'<=*ps && *ps<='Z')
			*ps+='a'-'A';
		ps++;
	}
}*/
void reg_test() {
  srand(time(0));
  uint32_t sample[8];
  uint32_t pc_sample = rand();
  cpu.pc = pc_sample;

  int i;
  for (i = R_EAX; i <= R_EDI; i ++) {
    sample[i] = rand();
    reg_l(i) = sample[i];
    assert(reg_w(i) == (sample[i] & 0xffff));
  }

  assert(reg_b(R_AL) == (sample[R_EAX] & 0xff));
  assert(reg_b(R_AH) == ((sample[R_EAX] >> 8) & 0xff));
  assert(reg_b(R_BL) == (sample[R_EBX] & 0xff));
  assert(reg_b(R_BH) == ((sample[R_EBX] >> 8) & 0xff));
  assert(reg_b(R_CL) == (sample[R_ECX] & 0xff));
  assert(reg_b(R_CH) == ((sample[R_ECX] >> 8) & 0xff));
  assert(reg_b(R_DL) == (sample[R_EDX] & 0xff));
  assert(reg_b(R_DH) == ((sample[R_EDX] >> 8) & 0xff));

  assert(sample[R_EAX] == cpu.eax);
  assert(sample[R_ECX] == cpu.ecx);
  assert(sample[R_EDX] == cpu.edx);
  assert(sample[R_EBX] == cpu.ebx);
  assert(sample[R_ESP] == cpu.esp);
  assert(sample[R_EBP] == cpu.ebp);
  assert(sample[R_ESI] == cpu.esi);
  assert(sample[R_EDI] == cpu.edi);

  assert(pc_sample == cpu.pc);
}

void isa_reg_display() {
	printf("EAX -->  0x%08x    %u\n", cpu.eax, cpu.eax);
	printf("ECX -->  0x%08x    %u\n", cpu.ecx, cpu.ecx);
	printf("EDX -->  0x%08x    %u\n", cpu.edx, cpu.edx);
	printf("EBX -->  0x%08x    %u\n", cpu.ebx, cpu.ebx);
	printf("ESP -->  0x%08x    %u\n", cpu.esp, cpu.esp);
	printf("EBP -->  0x%08x    %u\n", cpu.ebp, cpu.ebp);
	printf("ESI -->  0x%08x    %u\n", cpu.esi, cpu.esi);
	printf("EDI -->  0x%08x    %u\n", cpu.edi, cpu.edi);
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
//	strlwr(s);
	if (!strcmp(s, "$eax")) return (uint32_t) cpu.gpr[0]._32;
	else if (!strcmp(s, "$ecx")) return (uint32_t) cpu.gpr[1]._32;
	else if (!strcmp(s, "$edx")) return (uint32_t) cpu.gpr[2]._32;
	else if (!strcmp(s, "$ebx")) return (uint32_t) cpu.gpr[3]._32; 
	else if (!strcmp(s, "$esp")) return (uint32_t) cpu.gpr[4]._32;
	else if (!strcmp(s, "$ebp")) return (uint32_t) cpu.gpr[5]._32;
	else if (!strcmp(s, "$esi")) return (uint32_t) cpu.gpr[6]._32;
	else if (!strcmp(s, "$edi")) return (uint32_t) cpu.gpr[7]._32; 
	else if (!strcmp(s, "$ax")) return (uint32_t) cpu.gpr[0]._16; 
	else if (!strcmp(s, "$cx")) return (uint32_t) cpu.gpr[1]._16; 
	else if (!strcmp(s, "$dx")) return (uint32_t) cpu.gpr[2]._16; 
	else if (!strcmp(s, "$bx")) return (uint32_t) cpu.gpr[3]._16; 
	else if (!strcmp(s, "$sp")) return (uint32_t) cpu.gpr[4]._16; 
	else if (!strcmp(s, "$bp")) return (uint32_t) cpu.gpr[5]._16; 
	else if (!strcmp(s, "$si")) return (uint32_t) cpu.gpr[6]._16; 
	else if (!strcmp(s, "$di")) return (uint32_t) cpu.gpr[7]._16; 
	else if (!strcmp(s, "$al")) return (uint32_t) cpu.gpr[0]._8[0]; 
	else if (!strcmp(s, "$cl")) return (uint32_t) cpu.gpr[1]._8[0]; 
	else if (!strcmp(s, "$dl")) return (uint32_t) cpu.gpr[2]._8[0]; 
	else if (!strcmp(s, "$bl")) return (uint32_t) cpu.gpr[3]._8[0]; 
	else if (!strcmp(s, "$ah")) return (uint32_t) cpu.gpr[0]._8[1]; 
	else if (!strcmp(s, "$ch")) return (uint32_t) cpu.gpr[1]._8[1]; 
	else if (!strcmp(s, "$dh")) return (uint32_t) cpu.gpr[2]._8[1]; 
	else if (!strcmp(s, "$bh")) return (uint32_t) cpu.gpr[3]._8[1];
	else assert(0);
}
