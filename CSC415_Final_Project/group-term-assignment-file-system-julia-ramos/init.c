/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: init.c
 *
 * Description: Implementation file to initialize our file system
 *
 **************************************************************/

#include <stddef.h>  // "NULL"
#include <stdio.h>   // printf()
#include <stdlib.h>  // free()

#include "init.h"
#include "freeSpace.h"
#include "mfs.h"

/**
 * @brief Initializes the MBR, FreeSpace, and Root Directory
 * 
 * @param volumeSize Size of the volume
 * @param blockSize Size of 1 block
 * @return int 0 for success
 */
int FS_init(uint64_t volumeSize, uint64_t blockSize) {
    
    // Check if we are initialized already by reading the MBR
    struct MBR *MBRReadbuf =
        (struct MBR *)malloc(blockSize);  // For read testing
    int MBRblocksRead = LBAread(MBRReadbuf, 1, 0);
    if (MBRblocksRead != 1)
        printf("ERROR: MBR Blocks Read was not 1! Instead was %d\n",
               MBRblocksRead);

    if (MBRReadbuf->areWeInitialized != 1) {
        uint64_t blockCount = volumeSize / blockSize;
        
        // Initialize bit map
        bitMap_init(blockCount);  
    }

    // Update the bitmap
    int bitMapBlocks = 5;
    map = malloc(bitMapBlocks * blockSize);
    int mapblocksRead = LBAread(map, bitMapBlocks, 1);
    if (mapblocksRead != bitMapBlocks)
        printf("ERROR: map Blocks Read was not %d! Instead was %d\n",
               bitMapBlocks, mapblocksRead);
    // printBitMap(map);

    if (MBRReadbuf->areWeInitialized != 1) {
        uint64_t blockCount = volumeSize / blockSize;
        
        // Intialize MBR
        MBR_init(volumeSize, blockSize, blockCount);  
        
         // Initialize root directory
        init_RootDirectory(blockCount - 1); 
    }

    // Initialize currDirectoryPtr
    init_currDirPtr(1, 6);  

    free(MBRReadbuf);
    return 0;
}

/**
 * @brief Initializes the Master Boot Record
 * 
 * @param volumeSize Size of the volume
 * @param blockSize Size of 1 block
 * @param blockCount Count of blocks in the volume
 */
void MBR_init(uint64_t volumeSize, uint64_t blockSize, uint64_t blockCount) {
    
    // Initialize the MBR struct
    struct MBR *mbrBuf = (struct MBR *)malloc(blockSize);
    LBAread(mbrBuf, 1, 0);
    mbrBuf->mbrPosition = 0;
    mbrBuf->volumeSize = volumeSize;
    mbrBuf->blockSize = blockSize;
    mbrBuf->blockCount = blockCount;
    mbrBuf->signature = SIGNATURE;
    mbrBuf->rootDirPosition = 1;
    mbrBuf->currentDirPosition = 1;
    mbrBuf->areWeInitialized = 1;
    mbrBuf->bitMapStartPosition = 2;
    mbrBuf->bitMapSize = 1;
    strcpy(mbrBuf->volumeName, "TheSegFaults");

    // Write the MBR to the LBA
    printf("Writing MBR to the LBA... \n");
    int MBRblocksWrote = LBAwrite(mbrBuf, 1, 0);
    if (MBRblocksWrote != 1)
        printf("ERROR: MBR Blocks Wrote was not 1! Instead was %d\n",
               MBRblocksWrote);

    // Update the bitmap
    allocate_blocks(map, 0, 1);  
    int bitMapBlocks = 5;
    int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
    if (mapBlocksWrote != bitMapBlocks)
        printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
               bitMapBlocks, mapBlocksWrote);
    // printBitMap(map);

    // Test that we can read the MBR
    struct MBR *MBRReadbuf =
        (struct MBR *)malloc(blockSize);  // For read testing
    int MBRblocksRead = LBAread(MBRReadbuf, 1, 0);
    if (MBRblocksRead != 1)
        printf("ERROR: MBR Blocks Read was not 1! Instead was %d\n",
               MBRblocksRead);

    free(MBRReadbuf);  // For read testing
    free(mbrBuf);
}

/**
 * @brief Initializes the Root Directory
 * 
 * @param blockCount Count of blocks in the volume
 */
void init_RootDirectory(uint64_t blockCount) {
    
    // Initialize the root directory struct
    struct directoryInfo *rootDirectorybuf = malloc(MINBLOCKSIZE);
    rootDirectorybuf->freeBlock = 8;
    rootDirectorybuf->dirent.blocksForFile = blockCount - 1;
    rootDirectorybuf->dirent.dirEntStartLocation = 0;
    strcpy(rootDirectorybuf->dirent.timeCreated, "");
    strcpy(rootDirectorybuf->dirent.fileName, "/");
    rootDirectorybuf->dirent.fileType = 1;
    rootDirectorybuf->dirent.fileSize = blockCount * MINBLOCKSIZE;
    int rootNumOfBlocks = (int)ceil(sizeof(rootDirectorybuf) / 512.0);
    int rootLocation = getNextFreeBlocks(map, rootNumOfBlocks);
    rootDirectorybuf->currDirectoryLocation = rootLocation;
    rootDirectorybuf->parentDirectoryLocation = rootLocation;
    static const uint64_t initInodes[50] = {0};
    memcpy(rootDirectorybuf->inodes, initInodes, sizeof initInodes);

    // Write the root dir to the LBA
    printf("Writing Root Directory to the LBA... \n");
    int rootBlocksWrote =
        LBAwrite(rootDirectorybuf, rootNumOfBlocks, rootLocation);
    if (rootBlocksWrote != rootNumOfBlocks)
        printf("ERROR: Root dir Blocks Wrote was not %d! Instead was %d\n",
               rootNumOfBlocks, rootBlocksWrote);

    // Update the bitmap
    allocate_blocks(map, rootLocation, rootNumOfBlocks);
    int bitMapBlocks = 5;
    int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
    if (mapBlocksWrote != bitMapBlocks)
        printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
               bitMapBlocks, mapBlocksWrote);
    // printBitMap(map);

    // Test that we can read the root dir
    struct directoryInfo *rootDirReadbuf =
        (struct directoryInfo *)malloc(MINBLOCKSIZE);  // For read testing
    int rootDirBlocksRead =
        LBAread(rootDirReadbuf, rootNumOfBlocks, rootLocation);
    if (rootDirBlocksRead != rootNumOfBlocks)
        printf("ERROR: Root Dir Blocks Read was not %d! Instead was %d\n",
               rootNumOfBlocks, rootDirBlocksRead);
    free(rootDirReadbuf);  // For read testing

    free(rootDirectorybuf);
}

/**
 * @brief Initializes the current directory pointer
 * 
 * @param rootNumOfBlocks How many blocks the root directory takes up
 * @param rootLocation Location of the root directory
 */
void init_currDirPtr(uint64_t rootNumOfBlocks, uint64_t rootLocation) {
    
    // Update Current Directory Ptr
    currentDirectoryPtr = (struct directoryInfo *)malloc(MINBLOCKSIZE);
    uint64_t currDirBlocksRead =
        LBAread(currentDirectoryPtr, rootNumOfBlocks, rootLocation);
    if (currDirBlocksRead != rootNumOfBlocks)
        printf("ERROR: Curr Dir Blocks Read was not %ld! Instead was %ld\n",
               rootNumOfBlocks, currDirBlocksRead);
}

/**
 * @brief Initializes the bit map
 * 
 * @param myBlkCount Count of blocks in the volume
 */
void bitMap_init(int myBlkCount) {
    printf("Inside bitMap_init in freeSpace.c with myBlkCount = %d \n",
           myBlkCount);

    int blockSize = 512;
    int noOfBlocks = 19531;
    int arraySize = noOfBlocks / (sizeof(int) * 8) + 1;
    int bitMapBlocks = ((arraySize * sizeof(int)) / (blockSize)) + 1;

    // Initialize the bitmap to contain all free blocks
    int *bitMap = malloc(bitMapBlocks * blockSize);
    for (int i = 0; i < arraySize; i++) {
        bitMap[i] = 0;
    }

    // Update the bit map
    allocate_blocks(bitMap, 1, bitMapBlocks);
    printBitMap(bitMap);

    // Write the bit map to the LBA
    printf("Writing bit map to the LBA... \n");
    int mapBlocksWrote = LBAwrite(bitMap, bitMapBlocks, 1);
    if (mapBlocksWrote != bitMapBlocks)
        printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
               bitMapBlocks, mapBlocksWrote);

    free(bitMap);
}