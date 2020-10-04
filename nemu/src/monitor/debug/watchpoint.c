#include <regex.h>
#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP* new_wp(char *string) {
	WP *new;
	bool success;
	if(free_==NULL){
		printf("Error!free is null!\n");
		assert(0);
	}
	new=free_;
	free_=free_->next;
	strcpy(new->str,string);
	new->value=expr(string,&success);

	int status;
	int cflags =REG_EXTENDED;
	regmatch_t pmatch[1];
	const size_t nmatch=1;
	regex_t  reg;
	const char * pattern="\\$[eE][iI][pP][=][=]0x[0-9a-fA-F]{1,8}";
	regcomp(&reg,pattern,cflags);
	status=regexec(&reg,string,nmatch,pmatch,0);
	if(status==REG_NOMATCH)
		new->type='w';
	else if(status==0)
		new->type='b';

	new->Enb='Y';
	new->next=head;
	head=new;
	printf("The number is %d\n",new->NO);
	return new;
}

void free_wp(int number){
	WP *p,*q;
	q=NULL;
	p=head;
	while(p!=NULL&&p->NO!=number){
		q=p;
		p=p->next;
	}
	if(p==NULL){
		printf("Can't find!\n");
		assert(0);
	};
 	if(q==NULL){
		head=head->next;
		p->value=0;
		memset(p->str,0,sizeof(p->str));
		p->type=' ';
		p->Enb='N';
		p->next=free_;
		free_=p;
	}
	else{
		q->next=p->next;
		p->value=0;
		memset(p->str,0,sizeof(p->str));
		p->type=' ';
		p->Enb='N';
		p->next=free_;
		free_=p;
	}
	printf("Free the %d\n",p->NO);
	return;
}

