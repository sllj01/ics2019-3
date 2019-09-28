#include "cpu/exec.h"
#include "monitor/watchpoint.h"
#include "monitor/monitor.h"
extern WP* head;
extern WP* free_;
extern uint32_t expr(char*, bool*);

CPU_state cpu;

rtlreg_t s0, s1, t0, t1, ir;

/* shared by all helper functions */
DecodeInfo decinfo;

void decinfo_set_jmp(bool is_jmp) {
  decinfo.is_jmp = is_jmp;
}

void isa_exec(vaddr_t *pc);

vaddr_t exec_once(void) {
  decinfo.seq_pc = cpu.pc;
  isa_exec(&decinfo.seq_pc);
  update_pc();
  WP *temp=head;
  bool success=true;
  bool changed=false;
  while (temp!=NULL) {
	  success=true;
	  uint32_t a = expr(temp->expression, &success);
	  printf("a=%u\n", a);
	  if (a!=temp->value) {
		if (!changed) {
			printf("watchpoint value changed\n");
			printf("watchpoint  what      Old_value  New_value\n");
		}
		printf("%02d          %s        %u      %u", temp->NO, temp->expression, temp->value, a);
	  }
	  temp->value = a;
	  temp = temp->next;
  }

  if (changed) nemu_state.state = NEMU_STOP;
  else nemu_state.state = NEMU_RUNNING;
  return decinfo.seq_pc;
}
