#include "nemu.h"
#include <string.h>
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
uint32_t isa_reg_str2val(const char*, bool*);
uint32_t paddr_read(paddr_t, int);
extern bool success;
enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM10, TK_LCOM, TK_RCOM, TK_HEXADECIMAL, TK_REGNAME, TK_NOTEQUAL, TK_AND, TK_DEREF

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"\\-", '-'},
  {"\\*", '*'},
  {"\\/", '/'},
  {"==", TK_EQ},         // equal
  {"\\(", TK_LCOM},		// left combination mark
  {"\\)", TK_RCOM},		// right combination mark
  {"0x[\\dABCDEF]+", TK_HEXADECIMAL},
  {"[1-9]+", TK_NUM10},  // number in 10 jinzhi
  {"\\$[a-zA-Z]{2,3}", TK_REGNAME},
  {"!=", TK_NOTEQUAL},
  {"&&", TK_AND},
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
		  case TK_NOTYPE: break;
		  case TK_NUM10:case TK_HEXADECIMAL: case TK_REGNAME: strncpy(tokens[nr_token].str, substr_start, substr_len);
          default: tokens[nr_token].type = rules[i].token_type; nr_token++; 
        }

        break;
	  }
	}

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}


bool check_parentheses(int start,int end){
	if (tokens[start].type!=TK_LCOM || tokens[end].type!= TK_RCOM)
		return false;
	else
	{
		int count = 0;
		for (int p=start; p<=end; p++)
		{
			if (tokens[p].type==TK_LCOM) count++;
			else if (tokens[p].type==TK_RCOM) count--;
		}
		if (count==0) return true;
		else assert(0);
	}
}

int find_primary_operator(int start, int end){
	int flag=0;
	int out=start;
	for (int p=start; p<=end; p++)
	{	
		if (tokens[p].type==TK_EQ||tokens[p].type==TK_NOTEQUAL) return p;
		if (tokens[p].type==TK_LCOM) flag++;
		else if (tokens[p].type==TK_RCOM) flag--;
		else if (tokens[p].type=='+' || tokens[p].type=='-')
		{	if (flag==0) out = p;}
		else if (tokens[p].type=='*' || tokens[p].type=='/')
		{	if (flag==0&&tokens[out].type!='+'&&tokens[out].type!='-')
				out = p;
		}
		else if (tokens[p].type==TK_DEREF)
		{	if (flag==0&&tokens[out].type!='+'&&tokens[out].type!='-'&&tokens[out].type!='*'&&tokens[out].type!='/')
				out=p;
		}
	}
	return out;
}

uint32_t eval(int start, int end){
//	printf("click!\n");
	assert(start<=end);
	if (start==end){
		switch (tokens[start].type){
			case TK_NUM10: return (uint32_t) atoi(tokens[start].str);break;
			case TK_HEXADECIMAL: return (uint32_t) strtol(tokens[start].str, NULL, 16);break;
			case TK_REGNAME: return isa_reg_str2val(tokens[start].str, &success);break;
			default: assert(0);
			}}
	else if (check_parentheses(start, end)==true)
		return eval(start+1, end-1);
	else
	{	int op = find_primary_operator(start, end);
		if (tokens[op].type==TK_DEREF)
		{
			paddr_t addr = eval(op+1, end);
			return paddr_read(addr, 4);
		}
		else
		{
			uint32_t val1 = eval(start, op-1);
			uint32_t val2 = eval(op+1, end);
//			printf("%u\n%u\n", val1, val2);
			switch(tokens[op].type)
			{
				case '+': return val1+val2; break;
				case '-': return val1-val2; break;
				case '*': return val1*val2; break;
				case '/': return val1/val2; break;
				case TK_EQ: return val1==val2?1:0; break;
				case TK_NOTEQUAL: return val1==val2?0:1; break;
				case TK_AND: return val1&&val2; break;
				default: assert(0);
			}
		}
	}
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }
  for (int i=0; i<nr_token; i++){
	  if (tokens[i].type=='*' && (i==0||tokens[i-1].type==TK_LCOM||tokens[i-1].type==TK_EQ||tokens[i-1].type==TK_NOTEQUAL||tokens[i-1].type=='+'||tokens[i-1].type=='-'||tokens[i-1].type=='/'||tokens[i-1].type=='*'))
      tokens[i].type=TK_DEREF;
  }
  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1);
}
