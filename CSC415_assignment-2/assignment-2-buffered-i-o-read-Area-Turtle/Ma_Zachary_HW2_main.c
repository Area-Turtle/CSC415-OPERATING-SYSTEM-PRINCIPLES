/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub UserID: area-turtle
* Project: Assignment 2 – Buffered I/O
*
* File: Ma_Zachary_HW2_main.c
*
* Description: Program is to search for a file, parse through it 80 char at a time an record in 512 byte chunks using a buffer.
*
**************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "b_io.h"
#include "b_io.c"

#define BUFFERCNT 80

int main (int argc, char * argv[])
	{
	int readCount = 80058;
	char * buffer;
	b_io_fd fd;
	
	if (argc != 2)
		{
		printf ("Proper usage is: command filename\n");
		return -1;
		}
		
	buffer = malloc (BUFFERCNT + 2);	//allocate a little extra  space, need at least one for the null. (buffer becomes 82)
	/***  ToDo:  Check if malloc succeeds  ***/
	if (buffer == NULL) 
		{
		printf("Memory Failed");
		return 0;
		}
	else 
		{
		printf(argv[1]);
		printf("\nThis file exists\n");
		}	
	fd = b_open (argv[1]);
	//printf(fd);
	/***  ToDo:  Check if b_open succeeds  ***/
	if (fd = 0)
		{
		printf("b_open output: %d\n",fd);
		return 0;
		}
	else
		{
		printf("b_open error: %d\n",fd);
		//printf("%s",%s);
		}
		
	
	do
		{
		readCount = b_read (fd, buffer, BUFFERCNT);
		buffer[readCount] = '\0';
		printf ("%s\n", buffer);
		} while (readCount > 0);
	
	/***  TODO add b_close and free anything that needs freeing  ***/
	if (readCount<0)
		{
		b_close(fd);
		}
	return 0;
	}
	
