/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub UserID: area-turtle
* Project: Assignment 2 – Buffered I/O
*
* File: b_io.h
*
* Description:
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H

typedef int b_io_fd;
typedef struct mystruct mystruct; 

b_io_fd b_open (char * filename);
int b_read (b_io_fd fd, char * buffer, int count);
void b_close (b_io_fd fd);

#endif

