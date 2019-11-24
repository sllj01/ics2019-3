#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void*, size_t, size_t);
size_t ramdisk_write(const void*, size_t, size_t);
void init_ramdisk();
size_t get_ramdisk_size();
extern uint8_t ramdisk_start;


static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  Elf_Ehdr header;
  Log("----------------------------------------------------");
  Log("size of header is %d\n", sizeof(header));
  ramdisk_read(&header, 0, sizeof(header));
  uint32_t phdr_offset = header.e_phoff;
  uint16_t phnum = header.e_phnum;
  Log("num of headers is %d\n", phnum);
  uint16_t phentsize = header.e_phentsize;
  uint32_t program_entry = header.e_entry;
  Log("size of entrysize is %d\n", phentsize);

  Elf_Phdr phdr[phnum];
  // char buf[25000];
  ramdisk_read(&phdr, phdr_offset, phnum*phentsize);
  for (int index=0; index<phnum; index++) {
    uint32_t pt_load = phdr[index].p_type;
    if (pt_load != PT_LOAD) continue;
    uint32_t entry_offset = phdr[index].p_offset;
    uint32_t entry_filesize = phdr[index].p_filesz;
    uint32_t entry_memsize = phdr[index].p_memsz;
    uint32_t entry_vaddr = phdr[index].p_vaddr;
    // Log("entry_offset=%x, entry_filesize=%x, entry_memsize=%x, entry_vaddr=%x, type=%x\n------------------\n", entry_offset, entry_filesize, entry_memsize, entry_vaddr, pt_load);
    // assert(entry_filesize<25000);

    // ramdisk_read(buf, entry_offset, entry_filesize);
    // // printf("%p\n", (void*) entry_vaddr);
    // memcpy((void*) entry_vaddr, buf, entry_filesize);

    memcpy((void*) entry_vaddr, (void*) (ramdisk_start+entry_offset), entry_filesize);
    memset((void*) entry_vaddr+entry_filesize, 0, entry_memsize-entry_filesize);
  }
  return program_entry;
}

void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
