#ifndef __RTL_RTL_H__
#define __RTL_RTL_H__

#include "nemu.h"
#include "rtl/c_op.h"
#include "rtl/relop.h"
#include "rtl/rtl-wrapper.h"

extern rtlreg_t s0, s1, s2, t0, t1, ir;


void decinfo_set_jmp(bool is_jmp);
bool interpret_relop(uint32_t relop, const rtlreg_t src1, const rtlreg_t src2);

/* RTL basic instructions */

static inline void interpret_rtl_li(rtlreg_t* dest, uint32_t imm) {
  *dest = imm;
}

static inline void interpret_rtl_mv(rtlreg_t* dest, const rtlreg_t *src1) {
  *dest = *src1;
}

#define make_rtl_arith_logic(name) \
  static inline void concat(interpret_rtl_, name) (rtlreg_t* dest, const rtlreg_t* src1, const rtlreg_t* src2) { \
    *dest = concat(c_, name) (*src1, *src2); \
  } \
  /* Actually those of imm version are pseudo rtl instructions,
   * but we define them here in the same macro */ \
  static inline void concat(rtl_, name ## i) (rtlreg_t* dest, const rtlreg_t* src1, int imm) { \
    rtl_li(&ir, imm); \
    rtl_ ## name (dest, src1, &ir); \
  }

make_rtl_arith_logic(add)
make_rtl_arith_logic(sub)
make_rtl_arith_logic(and)
make_rtl_arith_logic(or)
make_rtl_arith_logic(xor)
make_rtl_arith_logic(shl)
make_rtl_arith_logic(shr)
make_rtl_arith_logic(sar)
make_rtl_arith_logic(mul_lo)
make_rtl_arith_logic(mul_hi)
make_rtl_arith_logic(imul_lo)
make_rtl_arith_logic(imul_hi)
make_rtl_arith_logic(div_q)
make_rtl_arith_logic(div_r)
make_rtl_arith_logic(idiv_q)
make_rtl_arith_logic(idiv_r)

static inline void interpret_rtl_div64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_div64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  uint64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  uint32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_idiv64_q(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend / divisor;
}

static inline void interpret_rtl_idiv64_r(rtlreg_t* dest,
    const rtlreg_t* src1_hi, const rtlreg_t* src1_lo, const rtlreg_t* src2) {
  int64_t dividend = ((uint64_t)(*src1_hi) << 32) | (*src1_lo);
  int32_t divisor = (*src2);
  *dest = dividend % divisor;
}

static inline void interpret_rtl_lm(rtlreg_t *dest, const rtlreg_t* addr, int len) {
  *dest = vaddr_read(*addr, len);
}

static inline void interpret_rtl_sm(const rtlreg_t* addr, const rtlreg_t* src1, int len) {
  vaddr_write(*addr, *src1, len);
}

static inline void interpret_rtl_host_lm(rtlreg_t* dest, const void *addr, int len) {
  switch (len) {
    case 4: *dest = *(uint32_t *)addr; return;
    case 1: *dest = *( uint8_t *)addr; return;
    case 2: *dest = *(uint16_t *)addr; return;
    default: assert(0);
  }
}

static inline void interpret_rtl_host_sm(void *addr, const rtlreg_t *src1, int len) {
  switch (len) {
    case 4: *(uint32_t *)addr = *src1; return;
    case 1: *( uint8_t *)addr = *src1; return;
    case 2: *(uint16_t *)addr = *src1; return;
    default: assert(0);
  }
}

static inline void interpret_rtl_setrelop(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, const rtlreg_t *src2) {
  *dest = interpret_relop(relop, *src1, *src2);
}

static inline void interpret_rtl_j(vaddr_t target) {
  cpu.pc = target;
  decinfo_set_jmp(true);
}

static inline void interpret_rtl_jr(rtlreg_t *target) {
  cpu.pc = *target;
  decinfo_set_jmp(true);
}

static inline void interpret_rtl_jrelop(uint32_t relop,
    const rtlreg_t *src1, const rtlreg_t *src2, vaddr_t target) {
  bool is_jmp = interpret_relop(relop, *src1, *src2);
  if (is_jmp) cpu.pc = target;
  decinfo_set_jmp(is_jmp);
}

void interpret_rtl_exit(int state, vaddr_t halt_pc, uint32_t halt_ret);


/* RTL pseudo instructions */

static inline void rtl_not(rtlreg_t *dest, const rtlreg_t* src1) {
  // dest <- ~src1
  //TODO();
  *dest = ~*src1;
}

static inline void rtl_sext(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- signext(src1[(width * 8 - 1) .. 0])
  //TODO();
  if (width==1)
  {
    t0 = *src1>>(width*8-1)&0b1;
    if (t0) *dest = 0xFFFFFF00|*src1;
    else *dest = 0x000000FF&*src1;
  }
  else if (width==2)
  {
    t0 = *src1>>(width*8-1)&0b1;
    if (t0) *dest = 0xFFFF0000|*src1;
    else *dest = 0x0000FFFF&*src1;
  }
  else if(width==4) {
    t0 = *src1>>(width*8-1)&0b1;
    if(t0) *dest = 0x00000000|*src1;
    else *dest = 0xFFFFFFFF&*src1;
  }
}

static inline void rtl_setrelopi(uint32_t relop, rtlreg_t *dest,
    const rtlreg_t *src1, int imm) {
  rtl_li(&ir, imm);
  rtl_setrelop(relop, dest, src1, &ir);
}

static inline void rtl_msb(rtlreg_t* dest, const rtlreg_t* src1, int width) {
  // dest <- src1[width * 8 - 1]
  //TODO();
  //------------------------------------------------------------------------------------------------------------------------------------
  // rtl_shri(src1, src1, width*8-1);
  // rtl_andi(src1, src1, 1);
  *dest = (*src1>>(width*8-1))&1;
}

static inline void rtl_mux(rtlreg_t* dest, const rtlreg_t* cond, const rtlreg_t* src1, const rtlreg_t* src2) {
  // dest <- (cond ? src1 : src2)
  TODO();
}

#include "isa/rtl.h"

#endif
