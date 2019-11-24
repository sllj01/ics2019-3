#include "rtl/rtl.h"
extern void rtl_push(const rtlreg_t*);

void raise_intr(uint32_t NO, vaddr_t ret_addr) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */
  rtl_push(&cpu.FLAGS);
  rtl_push(&cpu.cs);
  rtl_push(&ret_addr);
  // rtl_push(&NO);
  uint32_t base = cpu.idtr.base;
  // uint64_t describe = ((uint64_t)vaddr_read(base+NO*8, 4)<<32) | vaddr_read(base+NO*8+4, 4);
  // uint32_t jmp_des = (describe>>48)<<16 | (describe & 0xFFFF);
  uint32_t jmp_des = (vaddr_read(base+NO*8, 4)&(0x0000ffff)) | (vaddr_read(base+NO*8+4, 4)&(0xffff0000));
  printf("jmp des is %x\n", jmp_des);
  printf("test part\n");
  NO=0x81;
  jmp_des = (vaddr_read(base+NO*8, 4)&(0x0000ffff)) | (vaddr_read(base+NO*8+4, 4)&(0xffff0000));
  printf("jmp des for 0x81 is %x\n", jmp_des);
  NO=0x80;
  jmp_des = (vaddr_read(base+NO*8, 4)&(0x0000ffff)) | (vaddr_read(base+NO*8+4, 4)&(0xffff0000));
  printf("jmp des for 0x80 is %x\n", jmp_des);
  rtl_j(jmp_des);
}

bool isa_query_intr(void) {
  return false;
}
