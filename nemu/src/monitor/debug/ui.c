#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
void cpu_exec(uint64_t);
vaddr_t exec_once(void);
void isa_reg_display();
uint32_t paddr_read(paddr_t, int);
bool success=true;
extern WP *head;
extern WP *free_;
WP* new_wp();
void free_wp(WP*);

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

static int cmd_si(char *args){
	char *arg = strtok(NULL, " ");
	if (arg == NULL){
		exec_once();
	}
	else{
		int times = atoi(arg);
		for (int index=0; index<times; index++)
			exec_once();
	}
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	if (!strcmp(arg, "r"))
		isa_reg_display();
	else if (!strcmp(arg, "w")){
		printf("Num    Type          Value        What    ");
		WP* temp = head;
		while (temp!=NULL) {
			printf("%02d     hw watchpoint %u      %s\n", temp->NO, temp->value, temp->expression);
		temp = temp->next;
		}
	}
	else printf("arg is %s, this command has not been defined for now\n", arg);
	return 0;
}

static int cmd_x(char *args){
	char *arg1 = strtok(NULL, " ");
	char *arg2 = strtok(NULL, " ");
	paddr_t addr = (paddr_t) strtol(arg2, NULL, 16);	
	int op_time = atoi(arg1);
	printf("0x%x: \n", addr);
	for (int time = 0; time < op_time; time++, addr++)
	{	uint32_t outcome = paddr_read(addr, 4);
		printf("%u\n", outcome);
	}
	return 0;
}

static int cmd_p(char *args){
	char *arg;
	char *pp=strtok_r(args, " ", &arg);
	arg = strcat(pp, arg);
	//printf("%s\n%s",pp, arg);
	success=true;
	uint32_t num = expr(arg, &success);
	printf("%u\n", num);
	return 0;
}

static int cmd_w(char *args) {
	char *arg = strtok(NULL, " ");
	WP* temp = new_wp();
	success=true;
	strcpy(temp->expression, arg);
	temp->value = (uint32_t) expr(arg, &success);
	return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },
  { "si", "execute once or more steps", cmd_si},
  { "info", "show information about reg of watchpoints", cmd_info},
  { "x", "show the contents at a specific memory area", cmd_x},
  { "p", "calculate the value of a given expression", cmd_p},
  { "w", "set a watchpoint", cmd_w},
  /* TODO: Add more commands */

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
