/**************************************************************
 * Class:  CSC-415
 * Name: Professor Bierman
 * Student ID: N/A
 * Project: Basic File System
 *
 * File: fsLowDriver.c
 *
 * Description: This is a demo to show how to use the fsLow
 * 	routines.
 *
 **************************************************************/

#include <errno.h>
#include <fcntl.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "fsLow.h"
#include "mfs.h"

int main(int argc, char *argv[]) {
    char *filename;
    uint64_t volumeSize;
    uint64_t blockSize;
    int retVal;

    if (argc > 3) {
        filename = argv[1];
        volumeSize = atoll(argv[2]);
        blockSize = atoll(argv[3]);
    } else {
        printf("Usage: fsLowDriver volumeFileName volumeSize blockSize\n");
        return -1;
    }

    retVal = startPartitionSystem(filename, &volumeSize, &blockSize);
    printf("Opened %s, Volume Size: %llu;  BlockSize: %llu; Return %d\n",
           filename, (ull_t)volumeSize, (ull_t)blockSize, retVal);

    char *buf = malloc(blockSize * 2);
    char *buf2 = malloc(blockSize * 2);
    memset(buf, 0, blockSize * 2);
    strcpy(buf,
           "Now is the time for all good people to come to the aid of their "
           "countrymen\n");
    strcpy(&buf[blockSize + 10],
           "Four score and seven years ago our fathers brought forth onto this "
           "continent a new nation\n");
    LBAwrite(buf, 2, 0);
    LBAwrite(buf, 2, 3);
    LBAread(buf2, 2, 0);
    if (memcmp(buf, buf2, blockSize * 2) == 0) {
        printf("Read/Write worked\n");
    } else
        printf("FAILURE on Write/Read\n");

    free(buf);
    free(buf2);
    closePartitionSystem();
    return 0;
}
