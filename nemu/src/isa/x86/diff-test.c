#include "nemu.h"
#include "monitor/diff-test.h"

bool isa_difftest_checkregs(CPU_state *ref_r, vaddr_t pc) {
  
  if (pc+5 != ref_r->pc) {
    printf("input pc %x", pc);
    printf("ref pc %x", ref_r->pc);
    printf("wrong in pc\n");
    return false;
  }
  for (int index=0; index < 8; index++) {
    if (cpu.gpr[index]._32 != ref_r->gpr[index]._32) {
      printf("wrong in gpr[%d]\n", index);
      return false;
    }
  }
  if (cpu.eflags.CF!=ref_r->eflags.CF) return false;
  if (cpu.eflags.PF!=ref_r->eflags.PF) return false;
  if (cpu.eflags.AF!=ref_r->eflags.AF) return false;
  if (cpu.eflags.ZF!=ref_r->eflags.ZF) return false;
  if (cpu.eflags.SF!=ref_r->eflags.SF) return false;
  if (cpu.eflags.OF!=ref_r->eflags.OF) return false;
  return true;
}

void isa_difftest_attach(void) {
}
