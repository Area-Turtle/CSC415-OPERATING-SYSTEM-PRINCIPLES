/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub UserID: area-turtle
* Project: Assignment 3 – Simple Shell
*
* File: Ma_Zachary_HW3_main.c
*
* Description: Program is to take user prompts in the shell program, in which it should operate the same or similar to that of a regular linux interface.
*
**************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

//global values
#define BUFFER_SIZE 512
#define SPACER " \r\n"
#define MAX_ARGS 2
#define MAX_COUNT 1


int main(int argc, char **argv) 
	{
	char buffer[BUFFER_SIZE];  
	char *points[MAX_ARGS];  // each command will be in own space
	char *mark = NULL;
	int index;            // index position of buffer and later used for tokens variable
	pid_t pid;

	do {
		printf("Prompt$\t");
		index = 0;
		char character;
		
		// what user typed into buffer and terminates with a null character at the end
		while (1) 
			{
			character = getchar();
			if (character == '\n') 
				{
				buffer[index] = '\0';
				break;
				} 
			else 
				{
				buffer[index] = character;
				index++;
				}
			}
		
		index = 0;
		
		// put together characters in buffer into commands by using Spacer 
		mark = strtok(buffer, SPACER);
		while(mark != NULL) 
			{
			points[index] = mark;
			index++;
			mark = strtok(NULL, SPACER);			
			}
		
		
		points[index] = NULL;  // ending character is null
		
		 // if user enters exit, quit program
		if (strcmp(points[0], "exit") == 0)
			{
			return 0;
			}

		pid = fork();

		if (pid == 0) 
			{ // child process process fork()
			int exec_result = execvp(points[0], points);
			printf("Child %d with value at %d\n",getpid(), getppid());
			if (exec_result < 0) 
				{
				printf("Error in executing %s\n", points[0]);
				return 1;
				}
			
			return 0;

		} 
		else 
			{  // parent process fork()
			int exit_status;
			pid_t wait_result = waitpid(pid, &exit_status, 0);
			printf("Child %d with value at %d\n",getpid(),getppid());
			if (wait_result < 0) 
				{
				printf("Error in waiting %d\n", pid);
				return 1;
				}
			}

	} while (1);
	
	return 0;
	}
