#include "nemu.h"
#include <string.h>
#include <stdlib.h>
/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ, TK_NUM10, TK_LCOM, TK_RCOM 

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
  {"==", TK_EQ},         // equal
  {"\\(", TK_LCOM},		// left combination mark
  {"\\)", TK_RCOM},		// right combination mark
  {"[1-9]+", TK_NUM10}		// number in 10 jinzhi
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
		  case TK_NUM10: strncpy(tokens[nr_token].str, substr_start, substr_len);
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
		if (tokens[p].type==TK_LCOM) flag++;
		else if (tokens[p].type==TK_RCOM) flag--;
		else if (tokens[p].type=='+' || tokens[p].type=='-')
		{	if (flag==0) out = p;}
		else if (tokens[p].type=='*' || tokens[p].type=='/')
		{	if (flag==0&&tokens[out].type!='+'&&tokens[out].type!='-')
				out = p;
		}
	}
	return out;
}

uint32_t eval(int start, int end){
	assert(start<=end);
	if (start==end)
	 	return (uint32_t) atoi(tokens[start].str);
	else if (check_parentheses(start, end)==true)
		return eval(start+1, end-1);
	else
	{	int op = find_primary_operator(start, end);
		uint32_t val1 = eval(start, op-1);
		uint32_t val2 = eval(op+1, end);
		switch(tokens[op].type)
		{
			case '+': return val1+val2; break;
			case '-': return val1-val2; break;
			case '*': return val1*val2; break;
			case '/': return val1/val2; break;
			default: assert(0);
		}
	}
}


uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  return eval(0, nr_token-1);
}
