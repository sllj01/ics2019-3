#include "memory.h"
#include "proc.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
// 数据对齐！！！！！！
int mm_brk(uintptr_t brk, intptr_t increment) {
  extern PCB* current;
  uintptr_t new_brk = brk+increment;
  if (current->max_brk==0) {
    void* va =(void*) (brk>>12<<12);
    void* pa = new_page(1);
    _map(&current->as, va, pa, 1);
    current->max_brk = brk;
  }
  if (new_brk>current->max_brk) {
    printf("-------------pre_max_brk=%d, inc=%d, current_brk=%d\n", current->max_brk, increment, brk);
    int left = new_brk-current->max_brk;
    int res = (4096-(current->max_brk%4096))%4096;
    printf("res=%d\n", res);
    left = left-res;

    void* va = (void*) ((current->max_brk/4096+1)<<12);
    while(left > 0) {
      void* pa = new_page(1);
      printf("in mm_brk, va=%d, pa=%d\n", va, pa);
      _map(&current->as, va, pa, 1);
      va += (1<<12);
      left -= 4096;
    }
    current->max_brk = new_brk;
  }
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
