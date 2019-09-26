#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>
#define true 1
#define false 0
// this should be enough
static char buf[65536];
//static inline void gen_rand_expr() {
//  buf[0] = '\0';
//}
//

uint32_t gen_num(uint32_t top){
//	uint32_t product = (uint32_t)rand()%top;
//	int seed=time(0);
//	srand(seed);
	int product = rand()%top;
	return product;
}


void gen_opnum(int top){
//  uint32_t product = (uint32_t)rand()%top;
//	int seed = time(0);
//	srand(seed);
     uint32_t product = rand()%top;
	 char temp[100];
	 sprintf(temp, "%u", product);
	 strcat(buf, temp);
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

static inline void gen_rand_expr(int flag) {
	if (flag)
		buf[0] = '\0';
	switch (gen_num(3)){
		case 0: gen_opnum(9999); break;
		case 1: strcat(buf, "("); gen_rand_expr(0); strcat(buf, ")"); break;
		default: gen_rand_expr(0); gen_op(); gen_rand_expr(0); break;
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
	buf[0] = '\0';
    gen_rand_expr(true);
    printf("%s\n",buf);
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
//	printf("the process of writing result into a file is %d", _);
    pclose(fp);

    printf("%u %s\n", result, buf);
  }
  return 0;
}
