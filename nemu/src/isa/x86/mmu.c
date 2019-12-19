#include "nemu.h"

paddr_t page_translate(vaddr_t vaddr) {
  if ((cpu.CR0>>31) == 1) {
    uint32_t CR3 = cpu.CR3;
    // printf("CR3=%x\n", CR3);
    // printf("vaddr = %x\n", vaddr);
    uint32_t PG_TBL = paddr_read((CR3&~0xFFF)|(vaddr>>22<<2), 4);
    // printf("PG_TBL=%x\n", PG_TBL);
    if ((PG_TBL&0b1) != 1) assert(0);
    uint32_t PHY_ADDRESS = paddr_read((PG_TBL&~0xFFF)|(vaddr<<10>>22<<2), 4);
    // printf("PHY_ADDRESS=%x\n", PHY_ADDRESS);
    if ((PHY_ADDRESS&0b1) != 1) assert(0);
    return (PHY_ADDRESS&~0xFFF) | (vaddr&0xFFF);
  } else {
    return vaddr;
  }

}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if ((addr>>12)!=((addr+len)>>12)&&((addr&0xFFF)!=0xffc)) {
    printf("addr=%x, addr+len=%x\n", addr, addr+len);
    assert(0);
  }
  else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if ((addr>>12)!=((addr+len)>>12)&&(((addr+len)&0xFFF)!=0x000)) {
    printf("addr=%x, addr+len=%x\n", addr, addr+len);
    assert(0);
    // paddr_t phy_address1 = page_translate(addr);
    // paddr_t phy_address2 = page_translate(addr+len);
    // int len1 = (addr&0xFFFFF000)+0x00001000-addr;
    // int len2 = len-len1;
    // paddr_write(phy_address1, data, len1);
    // paddr_write(phy_address2, data>>(len*8), len2);
  }
  else {
    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data, len);
  }
}
