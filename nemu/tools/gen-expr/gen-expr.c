#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];
static int index = 0;
//static inline void gen_rand_expr() {
//  buf[0] = '\0';
//}
//

uint32_t gen_num(uint32_t top){
	return (uint32_t)rand()%top;
}

void gen_op() {
	switch (gen_num(4)){
		case 0: strcat(buf, "+"); break;
		case 1: strcat(buf, "-"); break;
		case 2: strcat(buf, "*"); break;
		case 3: strcat(buf, "/"); break;
		default: assert(0);
		}
}

static inline void gen_rand_expr() {
	buf[0] = '\0';
	switch (gen_num(3)) {
		case 0: char *temp; sprintf(temp, "%s", gen_num(0xFFFFFFFF)); strcat(buf, temp); break;
		case 1: strcat(buf, "("); gen_rand_expr(); strcat(buf, ")"); break;
		default: gen_rand_expr(); gen_op(); gen_rand_expr(); break;
	}
	return;
}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr();

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    fscanf(fp, "%d", &result);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
