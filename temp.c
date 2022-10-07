/**********************************************************************
 * Copyright (c) 2021-2022
 *  Sang-Hoon Kim <sanghoonkim@ajou.ac.kr>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTIABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 **********************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>

#include <string.h>

#include "types.h"
#include "list_head.h"
#include "parser.h"

/***********************************************************************
 * struct list_head history
 *
 * DESCRIPTION
 *   Use this list_head to store unlimited command history.
 */
extern struct list_head history;

struct entry{
	char *string;
	struct list_head list;
};
/***********************************************************************
 * run_command()
 *
 * DESCRIPTION
 *   Implement the specified shell features here using the parsed
 *   command tokens.
 *
 * RETURN VALUE
 *   Return 1 on successful command execution
 *   Return 0 when user inputs "exit"
 *   Return <0 on error
 */

int process_command(char * command);
	

void enqueue(char *string){
	struct entry *cmd = (struct entry*)malloc(sizeof(struct entry));

	if (cmd != NULL){
		list_add_tail(&(cmd->list), &history);
		cmd->string = malloc(sizeof(char)*80);
		if(cmd->string != NULL){
			strcpy(cmd->string,string);
		}
	}

	return;
}

void printHistory(void){
	struct entry* pos;
	int index = 0;
	list_for_each_entry(pos, &history, list){
		
		fprintf(stderr, "%2d: %s", index, pos->string);
		index++;
	}
}
void findEntry(int i, char* prev){
	struct entry* pos;
	
	int k=0;
	for (pos = list_first_entry(&history, __typeof__(*pos), list);k<i;k++){
		pos = list_next_entry(pos, list);
	}
	// printf("find string : %s",pos->string);
	strcpy(prev, pos->string);
	return;
}

int run_command(int nr_tokens, char * const tokens[])
{	
	char *argv[nr_tokens+1];
	for(int i=0;i<nr_tokens;i++){
		argv[i]=tokens[i];
	}
	argv[nr_tokens] = (char*) 0;

	const char *fil = tokens[0];

	pid_t pid;
	int status;

	pid = fork();

	//DO NOT USE system()
	if(pid<0){
		fprintf(stderr,"Fork error!\n");
	}
	if (strcmp(fil, "exit") == 0){
		return 0;
	}
	if(pid==0){
		if(strcmp(fil, "history")==0){
			printHistory();
		}else if(strcmp(fil, "!")==0){
			char* prev;
			
			findEntry(atoi(argv[1]), prev);
			process_command(prev);
		}
		else if(strcmp(fil, "cd")==0){
			if(nr_tokens == 1){
				int i = chdir(getenv("HOME"));
				if (i==0){
					printf("ok\n");
				}else{
					printf("no\n");
				}
			}else if(nr_tokens==2){
				if(strcmp(argv[1], "~")==0){
					int i = chdir(getenv("HOME"));
					if (i==0){
					}else{
					}
				}else{
					if(chdir(argv[1])){
						fprintf(stderr,"No directory\n");
					}
				}
			}else{
				fprintf(stderr, "Unable to execute %s\n", tokens[0]);
				exit(0);
			}
		}
		else{
			execvp(fil,argv);

		}
	}
	

	pid_t child = wait(&status);

	if(child<0){
		return -1;
	}else{
		return 1;
	}

	fprintf(stderr, "Unable to execute %s\n", tokens[0]);
	return -EINVAL;
	
}


/***********************************************************************
 * append_history()
 *
 * DESCRIPTION
 *   Append @command into the history. The appended command can be later
 *   recalled with "!" built-in command
 */
void append_history(char * const command)
{
	enqueue(command);
}


/***********************************************************************
 * initialize()
 *
 * DESCRIPTION
 *   Call-back function for your own initialization code. It is OK to
 *   leave blank if you don't need any initialization.
 *
 * RETURN VALUE
 *   Return 0 on successful initialization.
 *   Return other value on error, which leads the program to exit.
 */
int initialize(int argc, char * const argv[])
{
	return 0;
}


/***********************************************************************
 * finalize()
 *
 * DESCRIPTION
 *   Callback function for finalizing your code. Like @initialize(),
 *   you may leave this function blank.
 */
void finalize(int argc, char * const argv[])
{

}


/***********************************************************************
 * process_command(command)
 *
 * DESCRIPTION
 *   Process @command as instructed.
 */
int process_command(char * command)
{

	char *tokens[MAX_NR_TOKENS] = { NULL };
	int nr_tokens = 0;


	if (parse_command(command, &nr_tokens, tokens) == 0)
		return 1;

	return run_command(nr_tokens, tokens);
}
