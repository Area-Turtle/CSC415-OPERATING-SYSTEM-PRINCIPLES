/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: b_io.c
 *
 * Description: Buffered I/0 implentation for opening, reading, writing, and closing files 
 *
 **************************************************************/
#include "b_io.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "freeSpace.h"
#include "fsLow.h"
#include "mfs.h"

#define MAXFCBS 20
#define BUFSIZE 512

typedef struct b_fcb {
    int linuxFd;
    char *buf;
    int index;
    int buflen;
    int toWrite;
    char *node;
    int linuxOffset;
    int fileSize;

} b_fcb;

b_fcb fcbArray[MAXFCBS];

int startup = 0;

/**
 * @brief Initializes our open file array
 *
 */
void b_init() {
    for (int i = 0; i < MAXFCBS; i++) {
        fcbArray[i].linuxOffset = 0;
        fcbArray[i].linuxFd = -1;
    }

    startup = 1;
}

/**
 * @brief Get next available file descriptor in our open file array
 *
 * @return int The next available file descriptor
 */
int b_getFCB() {
    for (int i = 0; i < MAXFCBS; i++) {
        if (fcbArray[i].linuxFd == -1) {
            fcbArray[i].linuxFd = -2;
            return i;
        }
    }
}

/**
 * @brief Returns -1 if the file name already exists
 *
 * @param filename The file to check
 * @return int -1 if the file exists, location of the file if it exists
 */
int b_ifFileExist(char *filename) {
    // Check for the file in the parent's inodes
    uint64_t *inode = currentDirectoryPtr->inodes;
    for (int i = 0; i < 50; i++) {
        uint64_t filePosition = inode[i];
        if (inode != 0) {
            struct directoryInfo *searchBuf =
                (struct directoryInfo *)malloc(MINBLOCKSIZE);
            int searchBlocksRead = LBAread(searchBuf, 1, filePosition);

            // Check if the filename is the same
            if (strcmp(searchBuf->dirent.fileName, filename) == 0) {
                int location = searchBuf->currDirectoryLocation;
                free(searchBuf);
                return location;
            }
            free(searchBuf);
        }
    }
    return 0;
}

/**
 * @brief Creates the file in our file system or finds the file if it exists
 *
 * @param filename Filename to search/create
 * @param flags read/write flags
 * @return File System file descriptor
 */
int fs_open(char *filename, int flags) {
    // Check if the file exists
    uint64_t dirPosition = b_ifFileExist(filename);

    // If it exists, return its position
    if (dirPosition != 0) {
        return dirPosition + 1;

        // Create the file and return its new position
    } else {
        if (dirPosition == 0) {
            // Initialize the file struct
            struct directoryInfo *fileDirectoryEntry = malloc(MINBLOCKSIZE);
            int fileNumOfBlocks = (int)ceil(sizeof(fileDirectoryEntry) / 512.0);
            uint64_t positionInLBA = getNextFreeBlocks(map, fileNumOfBlocks);
            strcpy(fileDirectoryEntry->dirent.fileName, filename);
            fileDirectoryEntry->dirent.blocksForFile = 0;
            fileDirectoryEntry->dirent.fileSize = 0;
            fileDirectoryEntry->dirent.fileType = 0;
            strcpy(fileDirectoryEntry->dirent.timeCreated, "");
            fileDirectoryEntry->dirent.dirEntStartLocation = 0;
            fileDirectoryEntry->parentDirectoryLocation =
                currentDirectoryPtr->currDirectoryLocation;
            fileDirectoryEntry->currDirectoryLocation = positionInLBA;
            fileDirectoryEntry->freeBlock = 0;

            // Write the file to memory
            int fileBlocksWrote =
                LBAwrite(fileDirectoryEntry, fileNumOfBlocks, positionInLBA);
            if (fileBlocksWrote != fileNumOfBlocks)
                printf(
                    "ERROR: file metadata Blocks Wrote was not %d! Instead was "
                    "%d\n",
                    fileNumOfBlocks, fileBlocksWrote);

            // Upddate the bitmap
            allocate_blocks(map, positionInLBA, fileNumOfBlocks);
            int bitMapBlocks = 5;
            int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
            if (mapBlocksWrote != bitMapBlocks)
                printf(
                    "ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
                    bitMapBlocks, mapBlocksWrote);
            // printBitMap(map);

            // Update the parent
            for (int i = 0; i < 50; i++) {
                uint64_t dirEntStarPosition = currentDirectoryPtr->inodes[i];
                if (dirEntStarPosition == 0) {
                    currentDirectoryPtr->inodes[i] = positionInLBA;
                    break;
                }
            }
            uint64_t parentLocation =
                currentDirectoryPtr->currDirectoryLocation;
            int parentNumOfBlocks =
                (int)ceil(sizeof(currentDirectoryPtr) / 512.0);
            int parentWritten = LBAwrite(currentDirectoryPtr, parentNumOfBlocks,
                                         parentLocation);

            free(fileDirectoryEntry);
            return positionInLBA + 1;  //+1 next to metadata block
        }
    }
}

/**
 * @brief Opens the file for buffered reading/writing
 *
 * @param filename File to open
 * @param flags Flags for reading/writing
 * @return int Our buffered file descriptor
 */
int b_open(char *filename, int flags) {
    int fd;
    int returnFd;

    if (startup == 0) b_init();  // Initialize our system

    // lets try to open the file before I do too much other work

    fd = fs_open(filename, flags);
    if (fd == -1) return (-1);  // error opening filename

    // Should have a mutex here
    returnFd = b_getFCB();            // get our own file descriptor
                                      // check for error - all used FCB's
    fcbArray[returnFd].linuxFd = fd;  // Save the linux file descriptor
    fcbArray[returnFd].linuxOffset = 0;
    fcbArray[returnFd].fileSize = 0;
    //	release mutex

    // allocate our buffer
    fcbArray[returnFd].buf = malloc(BUFSIZE);
    if (fcbArray[returnFd].buf == NULL) {
        // very bad, we can not allocate our buffer
        close(fd);                        // close linux file
        fcbArray[returnFd].linuxFd = -1;  // Free FCB
        return -1;
    }

    fcbArray[returnFd].buflen = 0;  // have not read anything yet
    fcbArray[returnFd].index = 0;   // have not read anything yet

    return (returnFd);  // all set
}

/**
 * @brief Performs buffered read operation
 *
 * @param fd //file descriptor
 * @param buffer //callers buffer
 * @param count //number of bytes to be read
 * @return int //returns number of bytes transferred from the callers buffer
 */
int b_read(int fd, char *buffer, int count) {
    int bytesRead;            // bytes to be read
    int bytesReturned;        // what will be returned
    int remain;               // number of bytes available to copy from buffer
    int part1, part2, part3;  // holds the potential copy lengths

    if (startup == 0) {
        b_init();  // Initialize our system
    }

    if ((fd < 0) || fd >= MAXFCBS) {
        return -1;  // invalid file descriptor
    }

    // File not open for this descriptor
    if (fcbArray[fd].linuxFd == -1) {
        return -1;
    }

    remain = fcbArray[fd].buflen - fcbArray[fd].index;

    part3 = 0;
    if (remain >= count) {  // enough in buffer
        part1 = count;
        part2 = 0;
    } else {
        part1 = remain;  // first read
        part2 = count - remain;
    }

    if (part1 > 0) {
        memcpy(buffer, fcbArray[fd].buf + fcbArray[fd].index, part1);
        fcbArray[fd].index = fcbArray[fd].index + part1;
    }

    if (part2 > 0) {
        //read into our buffer
        bytesRead = LBAread(fcbArray[fd].buf, 1,fcbArray[fd].linuxFd + fcbArray[fd].linuxOffset) * BUFSIZE;
        bytesRead = strlen(fcbArray[fd].buf);

        fcbArray[fd].linuxOffset += 1;

        fcbArray[fd].index = 0;
        fcbArray[fd].buflen = bytesRead;

        if (bytesRead < part2) { //not enough bytes
            part2 = bytesRead;
        }

        if (part2 > 0) {
            memcpy(buffer + part1 + part3, fcbArray[fd].buf + fcbArray[fd].index, part2);
            fcbArray[fd].index = fcbArray[fd].index + part2;
        }
    }
    bytesReturned = part1 + part2 + part3;

    return bytesReturned;
}

/**
 * @brief Performs buffered write operation
 *
 * @param fd //file discriptor
 * @param buffer //callers buffer
 * @param count //number of bytes to be written
 * @return int //returns number of bytes transferred from the callers buffer
 */
int b_write(int fd, char *buffer, int count) {
    fcbArray[fd].toWrite = 1;

    int bytesWritten;         // bytes to be written
    int bytesReturned;        // what will be returned
    int remain;               // number of bytes available to copy from buffer
    int part1, part2, part3;  // holds the potential copy lengths

    if (startup == 0) {
        b_init();  // Initialize our system
    }

    if ((fd < 0) || fd >= MAXFCBS) {
        return -1;  // invalid file descriptor
    }

    // File not open for this descriptor
    if (fcbArray[fd].linuxFd == -1) {
        return -1;
    }

    remain = BUFSIZE - fcbArray[fd].index;
    part3 = 0;
    if (remain >= count) { //enough in buffer
        part1 = count;
        part2 = 0;
    } else {
        part1 = remain; //first write
        part2 = count - remain;
    }

    if (part1 > 0) {
        memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer, part1);
        fcbArray[fd].buflen = part1;
        fcbArray[fd].index = fcbArray[fd].index + part1;
    }

    if (part2 > 0) {
        //write into our buffer
        bytesWritten = LBAwrite(fcbArray[fd].buf, 1,fcbArray[fd].linuxFd + fcbArray[fd].linuxOffset) * BUFSIZE;
        bytesWritten = strlen(fcbArray[fd].buf);
        fcbArray[fd].fileSize += bytesWritten;

        allocate_blocks(map, fcbArray[fd].linuxFd + fcbArray[fd].linuxOffset, 1);
        int bitMapBlocks = 5;
        int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
        if (mapBlocksWrote != bitMapBlocks) {
            printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n", bitMapBlocks, mapBlocksWrote);
        }
        // printBitMap(map);

        fcbArray[fd].linuxOffset++;

        fcbArray[fd].index = 0;
        fcbArray[fd].buflen = bytesWritten;

        if (bytesWritten < part2) {//not enough bytes
            part2 = bytesWritten;
        }

        if (part2 > 0) {
            memcpy(fcbArray[fd].buf + fcbArray[fd].index, buffer + part1 + part3, part2);
            fcbArray[fd].index = fcbArray[fd].index + part2;
        }
    }

    bytesReturned = part1 + part2 + part3;
    return bytesReturned;
}

/**
 * @brief //repositions file offset
 *
 * @param fd //file descriptor
 * @param offset //offset of location in the file
 * @param whence //sets the file offset
 * @return int //returns the offset location in bytes from the beginning of the
 * file
 */
int b_seek(int fd, off_t offset, int whence) {
    switch (whence) {
        
        case 1:
            fcbArray[fd].index += offset; //set file offset to current location plus offset
            break;

        case 2:
            fcbArray[fd].index = offset; //set file offset to offset

        case 3:
            fcbArray[fd].index = fcbArray[fd].fileSize + offset; //set file offset to file size plus offset
            break;

        default:
            break;
    }

    return (fcbArray[fd].index);
}

/**
 * @brief used to close file
 *
 * @param fd //file descriptor
 */
void b_close(int fd) {
    // Flush write
    if (fcbArray[fd].toWrite == 1 && fcbArray[fd].buflen > 0) {
        char *finalBuf = (char *)malloc(fcbArray[fd].index + 1);
        memcpy(finalBuf, fcbArray[fd].buf, fcbArray[fd].index);
        fcbArray[fd].fileSize += fcbArray[fd].index;

        // Write to memory
        LBAwrite(finalBuf, 1, fcbArray[fd].linuxFd + fcbArray[fd].linuxOffset);

        // Update the bitmap
        allocate_blocks(map, fcbArray[fd].linuxFd + fcbArray[fd].linuxOffset,
                        1);
        int bitMapBlocks = 5;
        int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
        if (mapBlocksWrote != bitMapBlocks)
            printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
                   bitMapBlocks, mapBlocksWrote);
        // printBitMap(map);

        fcbArray[fd].linuxOffset++;
        free(finalBuf);
    }

    // Update the file's size
    if (fcbArray[fd].toWrite == 1) {
        struct directoryInfo *file = malloc(MINBLOCKSIZE);
        LBAread(file, 1, fcbArray[fd].linuxFd - 1);
        file->dirent.fileSize = fcbArray[fd].fileSize;
        file->dirent.blocksForFile = (int)ceil(fcbArray[fd].fileSize / 512.0);
        file->dirent.dirEntStartLocation = fcbArray[fd].linuxFd;
        LBAwrite(file, 1, fcbArray[fd].linuxFd - 1);
        free(file);
    }

    fcbArray[fd].toWrite = 0;
    free(fcbArray[fd].buf);
    fcbArray[fd].buf = NULL;
    fcbArray[fd].linuxFd = -1;
}