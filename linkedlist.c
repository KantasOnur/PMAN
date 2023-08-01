#include "linkedlist.h"
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>


node_t *new_node(pid_t pid){


	node_t *node = (node_t*)malloc(sizeof(node_t));
	node->pid = pid;
	node->next = NULL;
	node->stat = 'r';
	return node;
}

node_t *add(node_t *head, pid_t pid){

	node_t *node = new_node(pid);

	if(head == NULL){
		head = node;
		head->next = NULL;
	}
	else{
		node_t *cur = head;
		while(cur->next != NULL){
			cur = cur->next;
		}
		cur->next = node;
	}
	return head;
}

node_t *remove_pid(node_t *head, pid_t pid){

	if(head->pid == pid){
		node_t *temp = head;
		head = head->next;
		free(temp);
	}
	else{
		node_t *temp = NULL;
		for(node_t *cur = head; cur; cur = cur->next){
			if(cur->next->pid == pid){
				temp = cur->next;
				cur->next = temp->next;
				free(temp);
				break;
			}
		}
	}
	return head;
}

char pid_status(node_t *head, pid_t pid){

	for(node_t *cur = head; cur; cur = cur->next){
		if(pid == cur->pid){
			return cur->stat;
		}
	}
	return '-';
}

node_t *updateStatus(node_t *head, pid_t pid, char status){

	for(node_t *cur = head; cur; cur = cur->next){
		if(cur->pid == pid){
			cur->stat = status;
		}	
	}
	return head;
}
