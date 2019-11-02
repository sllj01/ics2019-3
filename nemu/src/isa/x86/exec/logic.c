#include "cpu/exec.h"
#include "cc.h"

make_EHelper(test) {
  //TODO();
  rtl_and(&s0, &id_dest->val, &id_src->val);

  // printf("dest is %x, src is %x--------------------\n", id_dest->val, id_src->val);

  rtl_li(&ir, 0);
  rtl_set_OF(&ir);
  rtl_set_CF(&ir);
  rtl_update_ZFSF(&s0, id_dest->width);
  // printf("res is %x-----------------------\n", s0);
  // printf("after set zf, zf is %x----------------------\n", cpu.eflags.ZF);

  print_asm_template2(test);
}

make_EHelper(and) {
  //TODO();
  rtl_and(&s0, &id_dest->val, &id_src->val);
  // printf("dest value is %x, src value is %x, result is %x\n", id_dest->val, id_src->val, s0);
  operand_write(id_dest, &s0);

  rtl_li(&ir, 0);
  rtl_set_CF(&ir);
  rtl_set_OF(&ir);
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(and);
}

make_EHelper(xor) {
  //TODO();
  rtl_xor(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);
  
  rtl_li(&ir, 0);
  rtl_set_CF(&ir);
  rtl_set_OF(&ir);
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(xor);
}

make_EHelper(or) {
  //TODO();
  rtl_or(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_li(&ir, 0);
  rtl_set_OF(&ir);
  rtl_set_CF(&ir);
  rtl_update_ZFSF(&s0, id_dest->width);
  print_asm_template2(or);
}

make_EHelper(sar) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_sar(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->val);
  print_asm_template2(sar);
}

make_EHelper(shl) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shl(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->val);
  print_asm_template2(shl);
}

make_EHelper(rol) {
  int t = id_src->val;
  while(t) {
    rtl_get_CF(&s1);
    rtl_msb(&s0, &id_dest->val, id_dest->width);
    rtl_xor(&s1, &s0, &s1);
    rtl_set_OF(&s1);
    rtl_shli(&s1, &id_dest->val, 1);
    rtl_add(&s1, &s0, &s1);
    operand_write(id_dest, &s1);
    rtl_set_CF(&s0);
  }
}

make_EHelper(shr) {
  //TODO();
  // unnecessary to update CF and OF in NEMU
  rtl_shr(&s0, &id_dest->val, &id_src->val);
  operand_write(id_dest, &s0);

  rtl_update_ZFSF(&s0, id_dest->val);

  print_asm_template2(shr);
}

make_EHelper(setcc) {
  uint32_t cc = decinfo.opcode & 0xf;

  rtl_setcc(&s0, cc);
  operand_write(id_dest, &s0);

  print_asm("set%s %s", get_cc_name(cc), id_dest->str);
}

make_EHelper(not) {
  //TODO();
  rtl_not(&s0, &id_dest->val);
  operand_write(id_dest, &s0);

  print_asm_template1(not);
}

