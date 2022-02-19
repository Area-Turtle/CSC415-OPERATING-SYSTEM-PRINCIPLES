/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub UserID: area-turtle
* Project: Assignment 2 – Buffered I/O
*
* File: b_io.c
*
* Description: methods in which the program will open, read and, close text files
*
**************************************************************/

#include "b_io.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#define B_CHUNK_SIZE 512 //buffer size
#define MAX_OPEN_FILES 2
int areWeInitialized = 0;

int oftable[MAX_OPEN_FILES];
b_init()
	{
	//lock
	areWeInitialized = 1;
	for (int i = 0; i < MAX_OPEN_FILES; i++)
		{
		// placeholder
		oftable[i];
		}
	for (int x=0;x<20;x++)
		{
		printf("%d\n",oftable[x]);
		}
	}

b_io_fd b_open (char * filename)
	{
	if (areWeInitialized == 0)
		{
		b_init();
		printf("Text: \n");
		int fdmain= open("DecOfInd.txt", O_RDONLY);
		return fdmain;
		}	
	//*** TODO ***:  Write open function to return file descriptor
	//               You must use the Linux System Calls (open)
	//				 You may want to allocate the buffer here as well
	//				 But make sure every file has its own buffer
	
	}
	
int b_read (b_io_fd fd, char * buffer, int count)
	{
	//*** TODO ***:  Write buffered read function to return the data and # bytes read
	//               You must use the Linux System Calls (read) and you must buffer the data
	//				 in 512 byte chunks. i.e. your linux read must be in B_CHUNK_SIZE
	printf("%d",fd);
	printf("%s",B_CHUNK_SIZE);
	printf("%d",count);
	int sum = fd+*buffer+count;
	printf("Total data used: %d",sum);
	return sum;
	}
	
void b_close (b_io_fd fd)
	{
	//*** TODO ***:  Close the file and release the buffer
	//				 You must use Linux System Calls (close)

	
	fflush(fd);
	
	}
