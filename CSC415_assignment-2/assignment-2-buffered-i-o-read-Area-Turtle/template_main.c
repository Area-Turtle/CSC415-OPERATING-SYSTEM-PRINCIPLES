/**************************************************************
* Class:  CSC-415-0# Summer 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub UserID: area-turtle
* Project: Assignment 2 – Buffered I/O
*
* File: template_main.c
*
* Description: template for the main program
*
**************************************************************/

#include <stdio.h>
#include "b_io.h"

#define BUFFERCNT 80

int main (int argc, char * argv[])
	{
	int readCount;
	char * buffer;
	b_io_fd fd;
	
	if (argc != 2)
		{
		printf ("Proper usage is: command filename\n");
		return -1;
		}
		
	buffer = malloc (BUFFERCNT + 2);	//allocate a little extra space, need at least one for the null.
	
	/***  ToDo:  Check if malloc succeeds  ***/
	
	fd = b_open (argv[1]);
	
	/***  ToDo:  Check if b_open succeeds  ***/
	
	do
		{
		readCount = b_read (fd, buffer, BUFFERCNT);
		buffer[readCount] = '\0';
		printf ("%s\n", buffer);
		} while (readCount > 0);
	
	/***  TODO add b_close and free anything that needs freeing  ***/
	
	return 0;
	}
	
