#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

//-----------------------------------externs----------------------------
void cpu_exec(uint64_t);
vaddr_t exec_once(void);
void isa_reg_display();
uint32_t vaddr_read(vaddr_t, int);
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


//--------------------------------commands-----------------------------
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
		cpu_exec(1);
	}
	else{
		int times = atoi(arg);
		cpu_exec(times);
	}
	return 0;
}

static int cmd_info(char *args){
	char *arg = strtok(NULL, " ");
	if (!strcmp(arg, "r"))
		isa_reg_display();
	else if (!strcmp(arg, "w")){
		printf("Num    Type          Value        What    \n");
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
	vaddr_t addr = (vaddr_t) strtol(arg2, NULL, 16);	
	int op_time = atoi(arg1);
	printf("0x%x: \n", addr);
	for (int time = 0; time < op_time; time++, addr++)
	{	uint32_t outcome = vaddr_read(addr, 4);
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
	uint32_t aa = (uint32_t) expr(arg, &success);
	WP* temp = new_wp();
	success=true;
	strcpy(temp->expression, arg);
	temp->value = aa;
	printf("set watchpoint on %s\n", temp->expression);
	return 0;
}

static int cmd_d(char *args) {
	int index = atoi(strtok(NULL, " "));
	WP* temp=head;
	WP* product=NULL;
	while (temp!=NULL) {
		if (temp->NO==index) { product = temp; break;}
		temp = temp->next;
	}
	if (product==NULL) printf("this whatchpoint doesnt exist!\n");
	else
	{
		free_wp(product);
		printf("delete watchpoint %d\n", product->NO);
	}
	return 0;
}

//-----------------------difftest_attach and detach-----------------------
static int cmd_detach(char *args) {
  Log("detaching from difftest\n");
  extern void difftest_detach();
  difftest_detach();
  return 0;
}

static int cmd_attach(char *args) {
  Log("attaching to difftest");
  extern void difftest_attach();
  difftest_attach();
  return 0;
}

//----------------------------sanp------------------------------------
static int cmd_save(char* args) {
  char* path = strtok(NULL, " ");
  if (!path) {
    printf("file path cannot be NULL!\n");
  }
  Log("saving snap to %s\n", path);
  FILE* p = fopen(path, "w");
  for(int index=0; index<8; index++) {
  fprintf(p, "%u\n", cpu.gpr[index]._32);
    // printf("%d\n", ret);
  }
  fprintf(p, "%u\n", cpu.pc);
  fprintf(p, "%u\n", cpu.FLAGS);
  fprintf(p, "%u\n", cpu.cs);
  fclose(p);
  return 0;
}

static int cmd_load(char* args) {
  //--------------------------restore the context in DUT
  char* path = strtok(NULL, " ");
  if (!path) {
    printf("unable to find such file or directory!\n");
  }
  Log("loading snap from %s", path);
  FILE* p = fopen(path, "r");
  for (int index=0; index < 8; index++) {
    int ret = fscanf(p, "%u", &cpu.gpr[index]._32);
    assert(ret!=EOF);
  }
  int ret = fscanf(p, "%u", &cpu.pc);
  assert(ret!=EOF);
  ret = fscanf(p, "%u", &cpu.FLAGS);
  assert(ret!=EOF);
  ret = fscanf(p, "%u", &cpu.cs);
  // assert(ret!=EOF);

  //---------------------------restore the context in REF: here i simply synchronize the cpu by attaching
  #ifdef DIFF_TEST
  extern void (*ref_difftest_setregs)(const void *c);
  ref_difftest_setregs(&cpu);
  # endif
  return 0;
}


//----------------------------cmd table-------------------------------


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
  { "d", "delete a watpoint", cmd_d},
  { "detach", "detach from qemu", cmd_detach}, 
  { "attach", "attach to qemu", cmd_attach}, 
  { "save", "save snap to a specified file path", cmd_save},
  { "load", "load snap from a specified file path", cmd_load}, 
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
