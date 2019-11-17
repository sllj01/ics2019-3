#include "rtl/rtl.h"
extern void rtl_push(const rtlreg_t*);

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.FLAGS);
  rtl_push(&cpu.cs);
  rtl_push(&cpu.pc);
  uint32_t base = IDTR.base;
  // uint64_t describe = ((uint64_t)vaddr_read(base+NO*8, 4)<<32) | vaddr_read(base+NO*8+4, 4);
  // uint32_t jmp_des = (describe>>48)<<16 | (describe & 0xFFFF);
  uint32_t jmp_des = vaddr_read(base+NO*8, 2)<<16 | vaddr_read(base+NO*8+6, 2);
  rtl_j(jmp_des);
}

bool isa_query_intr(void) {
  return false;
}
