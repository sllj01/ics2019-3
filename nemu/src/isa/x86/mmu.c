#include "nemu.h"

paddr_t page_translate(vaddr_t vaddr) {
  uint32_t CR3 = cpu.CR3;
  printf("CR3=%x\n", CR3);
  printf("vaddr = %x\n", vaddr);
  uint32_t PG_TBL = paddr_read(CR3+(vaddr>>22), 4);
  printf("PG_TBL=%x\n", PG_TBL);
  if ((PG_TBL&0b1) != 1) assert(0);
  uint32_t PHY_ADDRESS = paddr_read(PG_TBL+(vaddr<<10>>22), 4);
  printf("PHY_ADDRESS=%x\n", PHY_ADDRESS);
  if ((PHY_ADDRESS&0b1) != 1) assert(0);
  return PHY_ADDRESS + (vaddr&0xFFF);

}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  #ifdef HAS_VME
  if ((addr>>12)!=((addr+len)>>12)) assert(0);
  else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
  #endif
  return paddr_read(addr, len);
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  #ifdef HAS_VME
  if ((addr>>12)!=((addr+len)>>12)) assert(0);
  else {
    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data, len);
  }
  #endif
  paddr_write(addr, data, len);
}
