#include "common.h"
extern _Context* do_syscall(_Context*);
static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_YIELD: /*printf("trigger event yield\n");*/ break;
    case _EVENT_SYSCALL: /*printf("trigger event syscall\n");*/ do_syscall(c); break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return NULL;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
