/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: init.h
 *
 * Description: Header file to initialize our file system
 *
 **************************************************************/

#ifndef _INIT_H
#define _INIT_H

#include "fsLow.h"

#define SIGNATURE 0x536F636572743043

int MBRFlag;

struct MBR {
    uint64_t blockSize;
    uint64_t blockCount;
    uint64_t volumeSize;
    uint64_t signature;
    uint64_t bitMapStartPosition;
    uint64_t mbrPosition;
    uint64_t bitMapSize;
    uint64_t rootDirPosition;
    int areWeInitialized;
    int currentDirPosition;
    char volumeName[256];
};

/**
 * @brief Initializes the MBR, FreeSpace, and Root Directory
 * 
 * @param volumeSize Size of the volume
 * @param blockSize Size of 1 block
 * @return int 0 for success
 */
int FS_init(uint64_t volumeSize, uint64_t blockSize);

/**
 * @brief Initializes the Master Boot Record
 * 
 * @param volumeSize Size of the volume
 * @param blockSize Size of 1 block
 * @param blockCount Count of blocks in the volume
 */
void MBR_init(uint64_t volumeSize, uint64_t blockSize, uint64_t blockCount);

/**
 * @brief Initializes the Root Directory
 * 
 * @param blockCount Count of blocks in the volume
 */
void init_RootDirectory(uint64_t blockCount);

/**
 * @brief Initializes the current directory pointer
 * 
 * @param rootNumOfBlocks How many blocks the root directory takes up
 * @param rootLocation Location of the root directory
 */
void init_currDirPtr(uint64_t rootNumOfBlocks, uint64_t rootLocation);

/**
 * @brief Initializes the bit map
 * 
 * @param myBlkCount Count of blocks in the volume
 */
void bitMap_init(int myBlkCount);

#endif