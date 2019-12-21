#include "proc.h"
#include <elf.h>

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

#define vaddr_t uint32_t
#define paddr_t uint32_t

typedef __off_t off_t;
size_t ramdisk_read(void*, size_t, size_t);
size_t ramdisk_write(const void*, size_t, size_t);
void init_ramdisk();
size_t get_ramdisk_size();
extern uint8_t ramdisk_start;
int fs_open(const char*, int, int);
off_t fs_lseek(int, __off_t, int);
int fs_close(int);
size_t fs_read(int, void*, size_t);

static uintptr_t loader(PCB *pcb, const char *filename) {
  //TODO();
  // Elf_Ehdr header;
  // // Log("size of header is %d\n", sizeof(header));
  // ramdisk_read(&header, 0, sizeof(header));
  // uint32_t phdr_offset = header.e_phoff;
  // uint16_t phnum = header.e_phnum;
  // // Log("num of headers is %d\n", phnum);
  // uint16_t phentsize = header.e_phentsize;
  // uint32_t program_entry = header.e_entry;
  // // Log("size of entrysize is %d\n", phentsize);

  // Elf_Phdr phdr[phnum];
  // char buf[25000];
  // ramdisk_read(&phdr, phdr_offset, phnum*phentsize);
  // for (int index=0; index<phnum; index++) {
  //   uint32_t pt_load = phdr[index].p_type;
  //   if (pt_load != PT_LOAD) continue;
  //   uint32_t entry_offset = phdr[index].p_offset;
  //   uint32_t entry_filesize = phdr[index].p_filesz;
  //   uint32_t entry_memsize = phdr[index].p_memsz;
  //   uint32_t entry_vaddr = phdr[index].p_vaddr;
  //   // Log("entry_offset=%x, entry_filesize=%x, entry_memsize=%x, entry_vaddr=%x, type=%x\n------------------\n", entry_offset, entry_filesize, entry_memsize, entry_vaddr, pt_load);

  //   //血与痛的教训。buf切勿定义过大，否则极有可能缓冲区溢出导致覆盖IDTR。
  //   int left = entry_filesize;
  //   while (left>0) {
  //     if (left>=25000){
  //       ramdisk_read(buf, entry_offset, 25000);
  //       left-=25000;
  //     }
  //     else {
  //       ramdisk_read(buf, entry_offset, left);
  //       left-=left;
  //     }
  //     // printf("%p\n", (void*) entry_vaddr);
  //     memcpy((void*) entry_vaddr, buf, entry_filesize);
  //   }
  //   memset((void*) entry_vaddr+entry_filesize, 0, entry_memsize-entry_filesize);
  // }
  // return program_entry;

  // Log("-------------------------------------------------------------------------------");
  int fd = fs_open(filename, 0, 0);

  Elf_Ehdr header;
  // Log("size of header is %d\n", sizeof(header));
  fs_read(fd, (void*)&header, sizeof(header));
  // uint32_t phdr_offset = header.e_phoff;
  // uint16_t phnum = header.e_phnum;
  // Log("num of headers is %d\n", phnum);
  // uint16_t phentsize = header.e_phentsize;
  // uint32_t program_entry = header.e_entry;
  Log("-----------------------------program entry is %d\n", header.e_entry);

  Elf_Phdr phdr[header.e_phnum];
  char buf[4096];
  fs_lseek(fd, header.e_phoff, 0);
  fs_read(fd, (void*) &phdr, header.e_phnum*header.e_phentsize);

  for (int index=0; index<header.e_phnum; index++) {
    if (phdr[index].p_type != PT_LOAD) continue;
    // uint32_t entry_offset = phdr[index].p_offset;
    // uint32_t entry_filesize = phdr[index].p_filesz;
    // uint32_t entry_memsize = phdr[index].p_memsz;
    // uint32_t entry_vaddr = phdr[index].p_vaddr;
    // Log("entry_offset=%x, entry_filesize=%x, entry_memsize=%x, entry_vaddr=%x, type=%x\n------------------\n", entry_offset, entry_filesize, entry_memsize, entry_vaddr, pt_load);
    fs_lseek(fd, phdr[index].p_offset, 0);
    //血与痛的教训。buf切勿定义过大，否则极有可能缓冲区溢出导致覆盖IDTR。
    int left = phdr[index].p_filesz;
    int last_read = 0;
    vaddr_t va=0;
    paddr_t pa=0;
    while (left>0) {
      if (left>=4096){
        fs_read(fd, (void*) buf, 4096);
        va = phdr[index].p_vaddr+phdr[index].p_filesz-left;
        pa = (paddr_t) new_page(1);
        Log("set map va=%d, pa=%d\n", va, pa);
        _map(&pcb->as, (void*)va, (void*)pa, 1);
        memcpy((void*)pa, buf, 4096);
        left-=4096;
        last_read = 4096;
      }
      else {
        fs_read(fd, (void*) buf, left);
        va = phdr[index].p_vaddr+phdr[index].p_filesz-left;
        pa = (paddr_t) new_page(1);
        Log("set map va=%d, pa=%d\n", va, pa);
        _map(&pcb->as, (void*)va, (void*)pa, 1);
        memcpy((void*)pa, buf, left);
        last_read = left;
        left-=left;
      }
      // printf("%p\n", (void*) entry_vaddr);
    }

    left = phdr[index].p_memsz-phdr[index].p_filesz;
    if (last_read!=4096) {
      va = phdr[index].p_vaddr+phdr[index].p_filesz;
      int res = 4096-last_read;
      if (res>=left) {
        memset((void*)(pa+last_read), 0, left);
        continue;
      }
      else {
        memset((void*)(pa+last_read), 0, res);
        left-=res;
      }
      while (left>0) {
        va = phdr[index].p_vaddr + phdr[index].p_memsz-left;
        pa = (paddr_t) new_page(1);
        // Log("set map va=%d, pa=%d\n", va, pa);
        _map(&pcb->as, (void*)va, (void*)pa, 1);
        if (left>=4096) {
          memset((void*)pa, 0, 4096);
          left-=4096;
        }
        else {
          memset((void*)pa, 0, left);
          left-=left;
        }
      }
    }
  }
  fs_close(fd);
  return header.e_entry;
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
  _protect(&pcb->as);
  uintptr_t entry = loader(pcb, filename);
  printf("loader returns %d\n", entry);
  printf("user ptr=%d", pcb->as.ptr);

  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);

  pcb->cp = _ucontext(&pcb->as, stack, stack, (void *)entry, NULL);
}
