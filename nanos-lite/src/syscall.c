#include "common.h"
#include "syscall.h"

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
  printf("--------------%d", count);

  if (fd==1||fd==2) {
    for (int index=0; index<count; index++) _putc(*(buf+index));
  }
  return (uint32_t) count;
}



_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: Log("    sys_exit\n"); c->GPRx = sys_exit(c); break;     //////////which parameter should I pass to halt???   Conf
    case SYS_yield: Log("   sys_yield\n"); c->GPRx = sys_yield(c); break;
    case SYS_write: Log("   sys_write\n"); c->GPRx = sys_write(c); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}


