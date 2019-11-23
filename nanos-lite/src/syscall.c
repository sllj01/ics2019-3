#include "common.h"
#include "syscall.h"

extern void _exit(int);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
    case SYS_exit: _halt(c->GPR2); break;
    case SYS_yield: _yield(); c->GPRx = 0; break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }

  return c;
}
