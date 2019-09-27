#include "monitor/watchpoint.h"
#include "monitor/expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp() {
	if (free_==NULL) assert(0);
	else if (head==NULL) {
		WP* temp = free_;
		free_ = free_->next;
		head = temp;
		head->next=NULL;
		return temp;
	}
	else {
		WP* temp1 = free_;
		free_ = free_->next;
		WP* temp2 = head;
		while (temp2->next!=NULL) {
			temp2 = temp2->next;
		}
		temp2->next = temp1;
		temp2->next->next = NULL;
		return temp1;
	}
}


void free_wp(WP *wp) {
	if (wp==NULL) assert(0);
	if (head==NULL) assert(0);
	else if (wp==head) {
		WP* temp1 = head;
		head = head->next;
		if (free_==NULL) {free_=temp1; free_->next=NULL;}
		else {
			WP* temp2 = free_;
			free_ = temp1;
			free_->next = temp2;
		}
	}
	else {
		WP* temp1 = head;
		while (head->next!=wp) temp1 = temp1->next;
		WP* temp2 = free_;
		free_ = temp1->next;
		free_->next = temp2;
		temp1->next = temp1->next->next;
	}
}

