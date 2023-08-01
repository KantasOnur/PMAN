#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <sys/types.h>

typedef struct node_t{
	pid_t pid;
	struct node_t *next;
	char stat;
}node_t;

node_t *new_node(pid_t pid);
node_t *add(node_t *head, pid_t pid);
node_t *remove_pid(node_t *head, pid_t pid);
char pid_status(node_t *head, pid_t pid);
node_t *updateStatus(node_t *head, pid_t pid, char status);
#endif //LINKED_LIST_H
