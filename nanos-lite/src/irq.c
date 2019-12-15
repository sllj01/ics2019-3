#include "common.h"
extern _Context* do_syscall(_Context*);
extern _Context* schedule(_Context*);

static _Context* do_event(_Event e, _Context* c) {
  _Context* next = c;

  switch (e.event) {
    case _EVENT_YIELD: 
      // Log("trigger event yield\n");
      next = schedule(c);
      return next;
      // break;
    case _EVENT_SYSCALL: 
      // Log("trigger event syscall\n"); 
      do_syscall(c); 
      return NULL;
      // break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  // return next;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
