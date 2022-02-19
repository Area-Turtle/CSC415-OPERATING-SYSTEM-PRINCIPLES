/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: freeSpace.c
 *
 * Description: Implementation of freeSpace methods
 *
 **************************************************************/

#include <math.h>  // ceil()
#include <string.h> //memcpy

#include "freeSpace.h"
#include "fsLow.h"

/**
 * @brief Prints the bit map's occupied indexes
 * 
 * @param bitMap Bitmap to print
 */
void printBitMap(int* bitMap) {
    printf("[BITMAP] occupied indexes = ");
    int blocks = 19531;
    for (unsigned long long i = 0; i < blocks; i++) {
        if (TestBit(bitMap, i)) {
            printf("%lld ", i);
        }
    }
    printf("\n");
}

/**
 * @brief Allocates a given number of blocks at a given starting index
 * 
 * @param bitMap Bitmap to manipulate
 * @param index Starting position
 * @param count How many blocks to allocate
 * @return void* NULL
 */
void* allocate_blocks(int* bitMap, int index, int count) {
    for (int i = 0; i < count; i++) {
        unsigned long long position = index + i;
        SetBit(bitMap, position);
    }
    return NULL;
}

/**
 * @brief Frees a given number of blocks at a given starting index
 * 
 * @param bitMap Bitmap to manipulate
 * @param index Starting position
 * @param count How many blocks to free
 * @return void* NULL
 */
void* free_blocks(int* bitMap, int index, int count) {
    for (int i = 0; i < count; i++) {
        unsigned long long position = index + i;
        ClearBit(bitMap, position);
    }
    return NULL;
}

/**
 * @brief Get the index of the next free blocks
 * 
 * @param bitMap Bitmap to manipulate
 * @param numBlocks Number of free blocks to look for
 * @return uint64_t Starting index
 */
uint64_t getNextFreeBlocks(int* bitMap, int numBlocks) {
    int count = 0;
    int blocks = 19531;
    for (int i = 0; i < blocks; i++) {
        if (!TestBit(bitMap, i)) {
            count++;
        } else {
            count = 0;
        }
        if (count == numBlocks) {
            return i - numBlocks + 1;
        }
    }
    printf("Cannot find %d consecutive free blocks.\n", numBlocks);
    return -1;
}
