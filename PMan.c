#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <string.h>
#include "linkedlist.h"

node_t *head = NULL;

char *CUSTOM_COMMANDS[] = {

	"bg",
	"bglist",
	"bgkill",
	"bgstop",
	"bgstart",
	"pstat"

};

/*
 * Function: bg
 * --------------
 * Executes a program on the child process, and adds the program pid 
 * into a list in the parent process.
 *
 * argv: Array of input, each index containing an argument.
 */
void bg(char **argv){

	pid_t pid = fork();
	if(pid == 0){

		if(execvp(argv[0], argv) == -1){
			printf("Error: program (%s) failed to execute.\n", argv[0]);
			exit(-1);
		}
		exit(0);
	}
	else if(pid < 0){
		printf("Error: fork failed.\n");
		exit(1);
	}
	else{
		sleep(1);
		head = add(head, pid);
	}
}
/*
 * Function: cmdIndex
 * ------------------
 * Returns the index of which function was called
 * by the user. See, declared global constant.
 *
 * command: which custom command was called by the user
 * 
 * returns: the index of which the command resides in
 * the CUSTOM_COMMANDS array.
 */

int cmdIndex(char *command){

	for(int i = 0; i < 6; i++){	
		if(!strcmp(command, CUSTOM_COMMANDS[i])){
			return i;
		}
	}
	return -1;
}

/* 
 * Function: bglist
 * ----------------
 * Prints out the pid, and path of current background
 * programs that had been called by bg
 *
 */

void bglist(){


	int count = 0;
	char buf[1024];
	char link[1024];
	for(node_t *cur = head; cur; cur = cur->next){
		sprintf(link, "/proc/%d/exe", cur->pid);
		ssize_t len = readlink(link, buf, sizeof(buf));
		buf[len] = '\0';
		printf("%i: %s\n",cur->pid, buf);
		count++;
	}
	if(!count){
		printf("No background process is currently running.\n");
	}
	else{
		printf("Total background jobs: %i\n", count);
	}
}
/*
 * Function: bgkill
 * ----------------
 * Sends a term signal to the given pid.
 */
void bgkill(pid_t pid){
	
	if(pid_status(head, pid) == 's'){
		printf("Error: start the process (%i) to terminate.\n", pid);
	}
	else if(kill(pid, SIGTERM) != 0){
		printf("Error: process (%i) does not exist.\n", pid);
	}
	else{
		sleep(1);
	}
}

/*
 * Function: bgstop
 * ---------------
 * Sends a stop signal to the given pid
 */

void bgstop(pid_t pid){

	if(pid_status(head, pid) == 'r'){
		kill(pid, SIGSTOP);
		printf("Process (%i) has been stopped.\n", pid);
		head = updateStatus(head, pid, 's');
	}
	else if(pid_status(head, pid) == 's'){
		printf("Error: (%i) is already stopped.\n", pid);
	}
	else{
		printf("Error: process (%i) does not exist.\n", pid);
	}
}
/*
 * Function: bgstart
 * ------------------ 
 * Sends a cont signal to stopped processes.
 */
void bgstart(pid_t pid){

	if(pid_status(head, pid) == 's'){
		kill(pid, SIGCONT);
		printf("Process %i has been restarted.\n", pid);
		head = updateStatus(head, pid, 'r');
	}
	else if(pid_status(head, pid) == 'r'){
		printf("Error: process (%i) is already running.\n", pid);
	}
	else{
		printf("Error: process (%i) does not exist.\n", pid);
	}
}
/*
 * Function: pstat
 * ---------------
 * Displays various information about the given process
 */
void pstat(pid_t pid){

	char *line = NULL;
	size_t len;
	
	char file[1024];
	sprintf(file, "/proc/%d/stat", pid);
	
	FILE *fp = fopen(file, "r");

	sprintf(file, "/proc/%d/status", pid);
	FILE *fp2 = fopen(file, "r");
	if(!fp || !fp2){
		printf("Error: process (%d) does not exist.\n", pid);
	}
	else{
		getline(&line, &len, fp);
		line[strlen(line) - 1] = '\0';
		int index = 0;

		for(char *stat = strtok(line, " "); stat; stat = strtok(NULL, " ")){
			index++;
			if(index == 2){
				printf("comm: %s\n", stat);
			}
			if(index == 3){
				printf("state: %s\n", stat);
			}
			if(index == 14){
				printf("utime: %f\n", atof(stat)/sysconf(_SC_CLK_TCK));
			}
			if(index == 15){
				printf("stime: %s\n", stat);
			}
			if(index == 24){
				printf("rss: %s\n", stat);
				break;
			}
		}
		free(line);
		fclose(fp);
		index = 0;
		line = NULL;
		while(getline(&line, &len,fp2) != EOF){
			index++;
			if(!strncmp("vol", line, 3) || !strncmp("non", line, 3)){
				printf("%s", line);
			}
		}
		free(line);
	}

}

/*
 * Function: excuteInput
 * ---------------------
 * Executes the command and the arguments given from the user
 *
 * input: a string taken from stdin from the user
 */
void executeInput(char *input){

	char *argv[strlen(input)];
	char *command = strtok(input, " ");
	int index = cmdIndex(command);
	char *arg = strtok(NULL, " ");
	int i = 0;
	while(arg){
		argv[i] = arg;
		arg = strtok(NULL, " ");
		i++;
	}
	argv[i] = NULL;

	if(index >= 0){
		switch(index){
			case 0:
				bg(argv);
				break;
			case 1:
				bglist();
				break;
			case 2:
				bgkill(atoi(argv[0]));
				break;
			case 3:
				bgstop(atoi(argv[0]));
				break;
			case 4:
				bgstart(atoi(argv[0]));
				break;
			case 5:
				pstat(atoi(argv[0]));
				break;
				
		}
	}
	else{
		printf("PMan: > %s: command not found\n", command);
	}
}

/* Function: update
 * ----------------
 * Updates the states of all the child processes
 */
void update(){

	int status;
	pid_t pid;
	while((pid = waitpid(-1, &status, WNOHANG)) > 0){
		if(WIFEXITED(status) || WIFSIGNALED(status)){
			head = remove_pid(head, pid);
			printf("Process (%i) is terminated.\n", pid);
		}
	}
}

int main(void){
	
	char *input = NULL;
	while(1){
		input = readline("PMan: > ");
		update();
		if(!strlen(input)){
			printf("Error: enter a command.\n");
		}
		else{
		 	executeInput(input);
		}
		update();
	}
	return 0;
}
