#include "common.h"
#include "syscall.h"
typedef size_t off_t;
extern int fs_open(char*, int, int);
extern size_t fs_read(int, void*, size_t);
extern size_t fs_write(int, void*, size_t);
extern off_t fs_lseek(int, __off_t, int);
extern int fs_close(int);


size_t sys_exit(_Context* c) {
  _halt(c->GPR2);
  return c->GPRx;
}

size_t sys_yield(_Context* c) {
  _yield();
  return 0;
}

size_t sys_open(_Context* c) {
  char* pathname = (char*) c->GPR2;
  int flags = c->GPR3;
  int mode = c->GPR4;
  return fs_open(pathname, flags, mode);
}

size_t sys_write(_Context* c) {
  int fd = c->GPR2;
  void* buf = (void*) c->GPR3;
  size_t len = (size_t) c->GPR4;
  return fs_write(fd, buf, len);

  // int fd = c->GPR2;
  // char* buf = (char*) c->GPR3;
  // size_t len = (size_t) c->GPR4;
  // if (fd==1||fd==2) {
  //   for (int index=0; index<len; index++) {
  //     _putc(*(buf+index));
  //   }
  //   return len;
  // }
  // return -1;
}

size_t sys_read(_Context* c) {
  int fd = c->GPR2;
  void* buf = (void*) c->GPR3;
  size_t len = (size_t) c->GPR4;
  return fs_read(fd, buf, len);
}

size_t sys_lseek(_Context* c) {
  int fd = c->GPR2;
  __off_t offset = c->GPR3;
  int whence = c->GPR4;
  return fs_lseek(fd, offset, whence);
}

size_t sys_close(_Context* c) {
  int fd = c->GPR2;
  return fs_close(fd);
}

size_t sys_brk(_Context* c) {

  // uint32_t new_program_break = c->GPR2;
    // printf("%d, %d\n", end, new_program_break);
  return 0;////////////                  possibly return -1, but not discussed in PA3.  CONFUSED
}

size_t sys_execve(_Context* c) {
  extern void naive_uload(void*, const char*);
  const char* filename = (char*) c->GPR2;
  naive_uload(NULL, filename);
  return 0;
}

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: 
      // Log("    sys_exit\n"); 
      c->GPRx = sys_exit(c); break;     //////////which parameter should I pass to halt???   CONFUSED
    case SYS_yield: 
      // Log("   sys_yield\n"); 
      c->GPRx = sys_yield(c); break;
    case SYS_open:
      // Log("   sys_open");
      c->GPRx = sys_open(c); break;
    case SYS_read:
      // Log("   sys_read");
      c->GPRx = sys_read(c); break;
    case SYS_write: 
      // Log("   sys_write\n"); 
      c->GPRx = sys_write(c); break;
    case SYS_lseek:
      // Log("   sys_lseek");
      c->GPRx = sys_lseek(c); break;
    case SYS_close:
      // Log("   sys_close");
      c->GPRx = sys_close(c); break;
    case SYS_brk:
      // Log("    sys_brk\n");
      c->GPRx = sys_brk(c); break;
    case SYS_execve:
      // Log("    sys_execve\n");
      c->GPRx = sys_execve(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}


