#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  
  if (cpu.pc != ref_r->pc) {
    printf("input pc %x\n", cpu.pc);
    printf("ref pc %x\n", ref_r->pc);
    printf("wrong in pc\n");
    return false;
  }
  for (int index=0; index < 8; index++) {
    if (cpu.gpr[index]._32 != ref_r->gpr[index]._32) {
      printf("wrong in gpr[%d], value is %x\n", index, ref_r->gpr[index]._32);
      return false;
    }
  }
  // if (cpu.eflags.CF!=ref_r->eflags.CF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.eflags.PF!=ref_r->eflags.PF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.eflags.AF!=ref_r->eflags.AF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.eflags.ZF!=ref_r->eflags.ZF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.eflags.SF!=ref_r->eflags.SF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.eflags.OF!=ref_r->eflags.OF) {
  //   printf("wrong in CF\n");
  //   return false;
  // }
  // if (cpu.FLAGS!=ref_r->FLAGS) {
  //   printf("wrong in flags\n");
  //   printf("Flags in cpu %x", cpu.FLAGS);
  //   printf("Flags in ref_r %x", ref_r->FLAGS);
  //   return false;
  // }
  return true;
}

void isa_difftest_attach(void) {
  Log("ebtering isa_difftest_asstch!\n");
  ref_difftest_setregs(&cpu);
  ref_difftest_memcpy_from_dut(host_to_guest(0), (void*)0, 0x7c00);
  ref_difftest_memcpy_from_dut(host_to_guest(0x100000), (void*)0x100000, PMEM_SIZE);

}
