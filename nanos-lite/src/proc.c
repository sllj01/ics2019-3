#include "proc.h"
void naive_uload(void*, const char*);
void context_kload(PCB*, void*);
void context_uload(PCB*, char*);

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time!", j);
    j ++;
    _yield();
  }
}

void init_proc() {
  // context_kload(&pcb[0], (void*)hello_fun);
  context_uload(&pcb[0], "/bin/hello");
  context_uload(&pcb[1], "/bin/pal");
  context_uload(&pcb[2], "/bin/pal");
  context_uload(&pcb[3], "/bin/pal");
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here
  // naive_kload(&pcb[1], "/bin/init");

}

extern int fg_pcb;
_Context* schedule(_Context *prev) {
  // return NULL;

  // current->cp = prev;
  // current = &pcb[0];
  // return current->cp;
  static int counter=0;
  if (counter==0) {
    current->cp = prev;
    current = &pcb[0];
  }
  else {
    current->cp = prev;
    current = &pcb[fg_pcb];
  }
  counter = (counter+1)%1000;
  // current->cp = prev;
  // current = (current==&pcb[0]? &pcb[fg_pcb]:&pcb[0]);
  return current->cp;
}
