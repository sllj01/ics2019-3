#include "cpu/exec.h"
extern void raise_intr(uint32_t, vaddr_t);
//------------------------------------------------------------------------------------------
make_EHelper(lidt) {
  //TODO();
  uint16_t length = vaddr_read(id_dest->addr, 2);
  uint32_t base = vaddr_read(id_dest->addr+2, 4);
  IDTR.length = length;
  if (id_dest->width == 2) IDTR.base = base & 0xFFFFFF;
  else IDTR.base = base;
  print_asm_template1(lidt);
}

make_EHelper(mov_r2cr) {
  TODO();

  print_asm("movl %%%s,%%cr%d", reg_name(id_src->reg, 4), id_dest->reg);
}

make_EHelper(mov_cr2r) {
  TODO();

  print_asm("movl %%cr%d,%%%s", id_src->reg, reg_name(id_dest->reg, 4));

  difftest_skip_ref();
}

make_EHelper(int) {
  //TODO();
  printf("%x, %x\n", id_dest->val, id_dest->addr);
  raise_intr(id_dest->addr, decinfo.seq_pc);

  print_asm("int %s", id_dest->str);

  difftest_skip_dut(1, 2);
}

make_EHelper(iret) {
  rtl_pop(&s0);
  rtl_j(s0);
  rtl_pop(&s0);
  cpu.cs = s0;
  rtl_pop(&s0);
  cpu.FLAGS = s0;

  print_asm("iret");
}

uint32_t pio_read_l(ioaddr_t);
uint32_t pio_read_w(ioaddr_t);
uint32_t pio_read_b(ioaddr_t);
void pio_write_l(ioaddr_t, uint32_t);
void pio_write_w(ioaddr_t, uint32_t);
void pio_write_b(ioaddr_t, uint32_t);

make_EHelper(in) {
  //TODO();
  if (id_dest->width==1) {s0 = pio_read_b(id_src->val); operand_write(id_dest, &s0);}
  else if (id_dest->width==2) { s0 = pio_read_w(id_src->val); operand_write(id_dest, &s0);}
  else { s0 = pio_read_l(id_src->val); operand_write(id_dest, &s0);}
  print_asm_template2(in);
}

make_EHelper(out) {
  //TODO();
  if (id_dest->width==1) pio_write_b(id_dest->val, id_src->val);
  else if (id_dest->width==2) pio_write_w(id_dest->val, id_src->val);
  else pio_write_l(id_dest->val, id_src->val);


  print_asm_template2(out);
}
