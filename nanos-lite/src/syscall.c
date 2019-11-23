#include "common.h"
#include "syscall.h"
extern uint32_t end;

uint32_t sys_exit(_Context* c) {
  _halt(c->GPR2);
  return c->GPRx;
}

uint32_t sys_yield(_Context* c) {
  _yield();
  return 0;
}

uint32_t sys_write(_Context* c) {
  int fd = c->GPR2;
  char* buf = (char*) c->GPR3;
  size_t count = (size_t) c->GPR4;

  if (fd==1||fd==2) {
    for (int index=0; index<count; index++) _putc(*(buf+index));
  }
  return (uint32_t) count;
}

uint32_t sys_brk(_Context* c) {

  uint32_t new_program_break = c->GPR2;
    printf("%d, %d\n", end, new_program_break);
  end = new_program_break;
  return 0;////////////                  possibly return -1, but not discussed in PA3.  CONFUSED
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
    case SYS_write: 
      // Log("   sys_write\n"); 
      c->GPRx = sys_write(c); break;
    case SYS_brk:
      // Log("    sys_brk\n");
      c->GPRx = sys_brk(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}


