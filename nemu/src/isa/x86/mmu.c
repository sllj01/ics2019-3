#include "nemu.h"

paddr_t page_translate(vaddr_t vaddr) {
  if ((cpu.CR0>>31) == 1) {
    uint32_t CR3 = cpu.CR3;
    printf("CR3=%x\n", CR3);
    printf("vaddr = %x\n", vaddr);
    uint32_t PG_TBL = paddr_read((CR3&~0xFFF)|(vaddr>>22<<2), 4);
    printf("PG_TBL=%x\n", PG_TBL);
    if ((PG_TBL&0b1) != 1) assert(0);
    uint32_t PHY_ADDRESS = paddr_read((PG_TBL&~0xFFF)|(vaddr<<10>>22<<2), 4);
    printf("PHY_ADDRESS=%x\n", PHY_ADDRESS);
    if ((PHY_ADDRESS&0b1) != 1) assert(0);
    return (PHY_ADDRESS&~0xFFF) | (vaddr&0xFFF);
  } else {
    return vaddr;
  }

}

uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if ((addr>>12)!=((addr+len-1)>>12)) {
    printf("addr=%x, addr+len=%x\n", addr, addr+len);
    // assert(0);
    uint8_t bytes[len];
    for (int index=0; index<len; index++) {
      bytes[index] = isa_vaddr_read(addr+index, 1)&0xFF;
    }
    
    uint32_t ret=0;
    for (int index=0; index<len; index++) {
      ret = (ret<<8)|bytes[len-1-index];
    }
    return ret;
  }
  else {
    paddr_t paddr = page_translate(addr);
    return paddr_read(paddr, len);
  }
}

void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if ((addr>>12)!=((addr+len-1)>>12)) {
    printf("addr=%x, addr+len=%x\n", addr, addr+len);
    // assert(0);
    uint8_t bytes[4];

    for (int index=0; index<4; index++) {
      bytes[index] = data&0xFF;
      data = data>>8;
    }

    for (int index=0; index<len; index++) {
      isa_vaddr_write(addr+index, bytes[index], 1);
    }

  }
  else {
    paddr_t paddr = page_translate(addr);
    paddr_write(paddr, data, len);
  }
}
