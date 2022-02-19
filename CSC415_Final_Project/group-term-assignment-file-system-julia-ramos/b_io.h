/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: b_io.h
 *
 * Description: Interface of basic I/O functions
 *
 **************************************************************/

#ifndef _B_IO_H
#define _B_IO_H
#include <fcntl.h>
#include "mfs.h"

int b_open(char* filename, int flags);
int b_read(int fd, char* buffer, int count);
int b_write(int fd, char* buffer, int count);
int b_seek(int fd, off_t offset, int whence);
void b_close(int fd);

#endif