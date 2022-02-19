/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: freeSpace.h
 *
 * Description: Header file for free space bit map methods
 *
 **************************************************************/

#ifndef _FREESPACE_H
#define _FREESPACE_H

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif

#define SetBit(A, k) (A[(k / 32)] |= (1 << (k % 32)))
#define ClearBit(A, k) (A[(k / 32)] &= ~(1 << (k % 32)))
#define TestBit(A, k) (A[(k / 32)] & (1 << (k % 32)))

/**
 * @brief Prints the bit map's occupied indexes
 * 
 * @param bitMap Bitmap to print
 */
void printBitMap(int* bitMap);

/**
 * @brief Allocates a given number of blocks at a given starting index
 * 
 * @param bitMap Bitmap to manipulate
 * @param index Starting position
 * @param count How many blocks to allocate
 * @return void* NULL
 */
void* allocate_blocks(int* bitMap, int index, int count);

/**
 * @brief Frees a given number of blocks at a given starting index
 * 
 * @param bitMap Bitmap to manipulate
 * @param index Starting position
 * @param count How many blocks to free
 * @return void* NULL
 */
void* free_blocks(int* bitMap, int index, int count);

/**
 * @brief Get the index of the next free blocks
 * 
 * @param bitMap Bitmap to manipulate
 * @param numBlocks Number of free blocks to look for
 * @return uint64_t Starting index
 */
uint64_t getNextFreeBlocks(int* bitMap, int numBlocks);

#endif