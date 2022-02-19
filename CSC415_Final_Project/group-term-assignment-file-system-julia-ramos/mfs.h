/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: mfs.h
 *
 * Description: Header file for directory/file functions
 *
 **************************************************************/
#ifndef _MFS_H
#define _MFS_H

#include <dirent.h>
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

#include "b_io.h"
#define FT_REGFILE DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK DT_LNK

#ifndef uint64_t
typedef u_int64_t uint64_t;
#endif
#ifndef uint32_t
typedef u_int32_t uint32_t;
#endif

struct dirEntry {
    char timeCreated[20];  // time when the file/dir was created
    char fileName[16];  // file or directory name // TODO: Try to increase this
                        // size without errors
    uint64_t fileSize;  // size of file
    uint64_t blocksForFile;
    uint64_t fileType;             // directory or file ( 0 for file, 1 for dir)
    uint64_t dirEntStartLocation;  // start position of each directory entry
};

struct directoryInfo {
    uint64_t parentDirectoryLocation;  // parent directory position
    uint64_t currDirectoryLocation;    // current directory position
    uint64_t freeBlock;                // number of free blocks needed
    uint64_t inodes[50];     // inodes pointing to files in a directory
    struct dirEntry dirent;  // stores directory entry meta data
};

struct fs_diriteminfo {
    unsigned short d_reclen; /* length of this record */
    unsigned char fileType;
    char d_name[256]; /* filename max filename is 255 characters */
};

typedef struct {
    /*****TO DO:  Fill in this structure with what your open/read directory
     * needs  *****/
    unsigned short d_reclen; /*length of this record */
    unsigned short
        dirEntryPosition; /*which directory entry position, like file pos */
    uint64_t directoryStartLocation; /*Starting LBA of directory */
    char d_name[256];
    uint64_t offset;  // index into dir's inodes array
} fdDir;

struct directoryInfo *currentDirectoryPtr;
int *map;

int fs_mkdir(const char *pathname, mode_t mode);
int fs_rmdir(const char *pathname);
fdDir *fs_opendir(const char *name);
struct fs_diriteminfo *fs_readdir(fdDir *dirp);
int fs_closedir(fdDir *dirp);
char *getTimeCreated();
int fs_mkFile(const char *pathname, struct directoryInfo *fileInfo,
              uint64_t blocksNeeded, uint64_t lbaPosition);
int checkIfDirExist(char *currentDirName);
struct directoryInfo *getFile(char *fileName);
int fs_mvFile(char *fileName, char *Directory);

char *fs_getcwd(char *buf, size_t size);
int fs_setcwd(char *buf);       // linux chdir
int fs_isFile(char *path);      // return 1 if file, 0 otherwise
int fs_isDir(char *path);       // return 1 if directory, 0 otherwise
int fs_delete(char *filename);  // removes a file

struct fs_stat {
    off_t st_size;        /* total size, in bytes */
    blksize_t st_blksize; /* blocksize for file system I/O */
    blkcnt_t st_blocks;   /* number of 512B blocks allocated */
    time_t st_accesstime; /* time of last access */
    time_t st_modtime;    /* time of last modification */
    time_t st_createtime; /* time of last status change */

    /* add additional attributes here for your file system */
};

int fs_stat(const char *path, struct fs_stat *buf);

#endif