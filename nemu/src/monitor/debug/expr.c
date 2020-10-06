#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
	NOTYPE = 256, EQ=260, NUMBER = 230, UEQ=261, logical_AND=262, logical_OR=263,logical_NOT=264,REGISTER=265,VARIABLE=266,HEX=267,EIP=268,POINT=269
	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE},				// spaces
	{"\\+", '+'},					// plus 43
        {"-", '-'},                                   // minus 45 
        {"\\*", '*'},                                   // multiply 42 
        {"/", '/'},                                   // divide 47
        {"\\(", '('},                                   // ( 40
        {"\\)", ')'},                                   // ) 41
        {"\\b[0-9]+\\b", NUMBER},                       //number
	{"==", EQ},						// equal
	{"!=",UEQ},
	{"&&", logical_AND},
	{"\\|\\|", logical_OR},
	{"!", logical_NOT},
	{"0[xX][A-Fa-f0-9]{1,8}",HEX},
	{"\\$[a-dA-D][hlHL]|\\$[eE]?(ax|dx|cx|bx|bp|si|di|sp)",REGISTER},
	{"\\[Ee][Ii][Pp]",EIP},
	{"[a-zA-Z_][a-zA-Z0-9_]*",VARIABLE}
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
} Token;

Token tokens[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
				position += substr_len;

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
                                       case '+':tokens[nr_token].type='+';break;
                                       case '-':tokens[nr_token].type='-';break;
                                       case '*':tokens[nr_token].type='*';break;
                                       case '/':tokens[nr_token].type='/';break;
                                       case '(':tokens[nr_token].type='(';break;
                                       case ')':tokens[nr_token].type=')';break;
                                       case 260:tokens[nr_token].type=260;strcpy(tokens[nr_token].str,"==");break;
				       case 261:tokens[nr_token].type=261;strcpy(tokens[nr_token].str,"!=");break;
                                       case 262:tokens[nr_token].type=262;strcpy
(tokens[nr_token].str,"&&");break;
                                       case 263:tokens[nr_token].type=263;strcpy
(tokens[nr_token].str,"||");break;
				       case 264:tokens[nr_token].type=264;break;
                                       case 265:tokens[nr_token].type=265;
strncpy(tokens[nr_token].str,substr_start,substr_len);
break;
                                       case 266:tokens[nr_token].type=266;
strncpy(tokens[nr_token].str,substr_start,substr_len);
break;
                                       case 267:tokens[nr_token].type=267;
strncpy(tokens[nr_token].str,substr_start,substr_len);
break;
				       case 268:tokens[nr_token].type=268;break;
				       case 256:break;
                                       case 230:tokens[nr_token].type=230;
strncpy(tokens[nr_token].str,substr_start,substr_len);
break;
					default: nr_token--;break;
				}
				nr_token++;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}
	nr_token--;
	return true; 
}

bool check_parentheses(int p,int q){
        int i,cnt=0;
	if(tokens[p].type!='(' ||tokens[q].type!=')')
		return false;
        for(i=p;i<=q;i++){
                if(tokens[i].type=='(')
                        cnt++;
                else if(tokens[i].type==')')
                        cnt--;
                if((cnt==0)&&(i<q)){
                        return false;
                }
        }
	if(cnt!=0)
                 return false;
	return true;        
}

int pri(int a){
		switch(tokens[a].type){
			case '+':return 4;break;
			case '-':return 4;break;
			case '*':return 3;break;
			case '/':return 3;break;
			case 260:return 7;break;
			case 261:return 7;break;
			case 262:return 11;break;
			case 263:return 12;break;
			case 264:return 2;break;
			case 269:return 2;break;
			default:return -2;break;
		}
	return -2;
	}

int dominant_op(int p, int q) {
          int i,j,op=p,k=0;
	  int pr=-1;
          for(i=p;i<=q;i++){
                if(tokens[i].type=='('){
                        k++;
			i++;
			for(j=i+1;j<=q;j++){
				if(tokens[j].type=='(') k++;
				if(tokens[j].type==')') k--;
				if(k==0) {
					i=j;
					break;
				}
			}
		}
		else if(tokens[i].type ==230)continue;
		else if(pri(i)>=pr){
			pr=pri(i);
			printf("%d",pr);
			op=i;
		}
	}
	printf("%d%d%d\n",p,op,q);
       	printf("%c\n",tokens[op].type);
	return op;
}

int eval(int p, int q){
	int num=0;
        if(p>q) {
           assert(0);
        }
        else if(p == q){
           if(tokens[p].type==230){
                sscanf(tokens[p].str,"%d",&num);
                return num;
                }
	  else if(tokens[p].type==267){
		sscanf(tokens[p].str,"%x",&num);
		return num;
		}
	  else if(tokens[p].type==265){
            	int j=0,sl=1,sw=1;
		for(;j<8&&sl!=0&&sw!=0;j++){
			sl=strcmp(tokens[p].str+1,regsl[j]);
			sw=strcmp(tokens[p].str+1,regsw[j]);
		}
		if(sl==0)
		{
			num=cpu.gpr[j]._32;
			return num;
		}
		else if(sw==0)
			return cpu.gpr[j]._16;
		else {
			if(strcmp(tokens[p].str,"$al")==0)
				return reg_b(0);
                       if(strcmp(tokens[p].str+1,"cl")==0)
                                return reg_b(1);
                       if(strcmp(tokens[p].str+1,"dl")==0)
                                return reg_b(2);
                       if(strcmp(tokens[p].str+1,"bl")==0)
                                return reg_b(3);
                       if(strcmp(tokens[p].str+1,"ah")==0)
                                return reg_b(4);
                       if(strcmp(tokens[p].str+1,"ch")==0)
                                return reg_b(5);
                       if(strcmp(tokens[p].str+1,"dh")==0)
                                return reg_b(6);
                       if(strcmp(tokens[p].str+1,"bh")==0)
                                return reg_b(7);
		}
		if(j==8)
			assert(0);
	}
	else if(tokens[p].type==268)
		return cpu.eip;
	else
		assert(0);
        }
        else if(check_parentheses(p,q)==true) {
//	  printf("true");
          return eval(p+1,q-1);
        }
        else {
	  if((q-p==1)&&tokens[p].type=='-')
		return 0-eval(q,q);
	  if(((q-p==1)||(tokens[p+1].type=='('&&tokens[q].type==')'))&&tokens[p].type==264){
		num=eval(p+1,q);
		return !num;
	}
          if(((q-p==1)||(tokens[p+1].type=='('&&tokens[q].type==')'))&&tokens[p].type=='*'){
                tokens[p].type=269;
 		return swaddr_read(eval(p+1,q),4);
        }

          int op=dominant_op(p,q);
          int val1=eval(p,op-1);
          int val2=eval(op+1,q);

        switch(tokens[op].type) {
          case '+':return val1+val2;
          case '-':return val1-val2;
          case '*':return val1*val2;
          case '/':return val1/val2;
	  case 260:
		if(val1==val2)
			return 1;
		else
			return 0;
	  case 261:
		if(val1!=val2)
			return 1;
		else
			return 0;
	  case 262:
		if(val1&&val2)
			return 1;
		else
			return 0;
	  case 263:
		if(val1||val2)
			return 1;
		else
 			return 0;
          default:assert(0);
        }
      }
	return 0;
    }

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
        if(make_token(e)) {
    		*success = true;
		return eval(0,nr_token);
	}
	/* TODO: Insert codes to evaluate the expression. */
	panic("please implement me");
	return 0;
}
