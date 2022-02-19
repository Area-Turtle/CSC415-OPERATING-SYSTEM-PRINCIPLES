/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name:Zachary Ma
* Student ID:916750281
* Project: Assignment 5 – Buffered I/O
*
* File: b_io.h
*
* Description: buffer-i-o abstract b_open/read/write/close
*
**************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>

int b_open (char * filename, int flags);
int b_read (int fd, char * buffer, int count);
int b_write (int fd, char * buffer, int count);
void b_close (int fd);

#endif

