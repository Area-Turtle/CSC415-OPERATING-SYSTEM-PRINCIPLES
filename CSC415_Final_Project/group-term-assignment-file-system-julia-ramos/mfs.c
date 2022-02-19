/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: mfs.c
 *
 * Description: Implementation file for directory/file functions
 *
 **************************************************************/

#include "mfs.h"

#include "freeSpace.h"  // allocate_blocks()
#include "fsLow.h"      // MINBLOCKSIZE
#include "path.h"       // getCurrDir()

#define FT_REGFILE DT_REG
#define FT_DIRECTORY DT_DIR
#define FT_LINK DT_LNK

#define DIRECTORY_SIGNATURE 0x536F636572743045
#define FILE_SIGNATURE 0x536F636572743046

/**
 * @brief Get the current time
 * 
 * @return char* Current time string
 */
char *getCurrTime() {
    time_t t;
    time(&t);
    char *s = malloc(20);
    struct tm *tm = localtime(&t);
    strftime(s, 20, "%Y-%m-%d %H:%M:%S", tm);
    return s;
}

/**
 * @brief Checks if the directory exists
 * 
 * @param currentDirName File/dir name
 * @return int 0 if the file/dir exists
 */
int checkIfDirExist(char *currentDirName) {
    if (strlen(currentDirName) != 0) {
        // get inodes array to check if the directory meta data exists
        uint64_t *getInodePosition = currentDirectoryPtr->inodes;

        for (int i = 0; i < 50; i++) {
            uint64_t dirEntryStartLocation = getInodePosition[i];

            if (getInodePosition == 0) {
                continue;
            }
            struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
            // get metadata of the directory at the position found
            LBAread(tempDirectory, 1, dirEntryStartLocation);

            if (strcmp(tempDirectory->dirent.fileName, currentDirName) == 0) {
                printf(
                    "Error: directory name already exists. Please give a "
                    "new directory name.\n");
                return -1;
            }
            free(tempDirectory);
        }
        return 0;

    } else {
        printf("Error: Please provide a directory name.\n");
        return -1;
    }
}

/**
 * @brief Makes a directory
 * 
 * @param pathname Directory name
 * @param mode Mode
 * @return int 0 for success
 */
int fs_mkdir(const char *pathname, mode_t mode) {
    
    // Get the directory name
    char *path = malloc(strlen(pathname) + 1);
    strcpy(path, pathname);
    char *currentDirectoryName = getCurrDir(path);
    free(path);

    // check if the pathname exists
    if (checkIfDirExist(currentDirectoryName) == 0) {  // NEEDS DEBUG

        // Malloc space for the new directory
        struct directoryInfo *newDirectory = malloc(MINBLOCKSIZE);
        if (newDirectory == NULL) {
            printf("Could not allocate memory. \n");
            return -1;  // malloc failed
        }

        // Initializing fields of the new directory
        strcpy(newDirectory->dirent.fileName, currentDirectoryName);
        uint64_t freeBlock = currentDirectoryPtr->freeBlock;
        newDirectory->dirent.fileType = 1;  // 1 for directory
        newDirectory->dirent.blocksForFile = 1;
        strcpy(newDirectory->dirent.timeCreated, getCurrTime());
        newDirectory->dirent.fileSize = 0;
        uint64_t parentLocation = currentDirectoryPtr->currDirectoryLocation;
        newDirectory->parentDirectoryLocation = parentLocation;
        newDirectory->freeBlock = freeBlock + 2;
        static const uint64_t initInodes[50] = {0};
        memcpy(newDirectory->inodes, initInodes, sizeof initInodes);
        int dirNumOfBlocks = (int)ceil(sizeof(newDirectory) / 512.0);
        uint64_t positionInLBA = getNextFreeBlocks(map, dirNumOfBlocks);
        newDirectory->currDirectoryLocation = positionInLBA;
        newDirectory->dirent.dirEntStartLocation = positionInLBA;
        for (int i = 0; i < 50; i++) {
            uint64_t dirEntStarPosition = currentDirectoryPtr->inodes[i];
            if (dirEntStarPosition == 0) {
                currentDirectoryPtr->inodes[i] = positionInLBA;
                break;
            }
        }

        // Update the parent directory
        int parentNumOfBlocks = (int)ceil(sizeof(currentDirectoryPtr) / 512.0);
        int parentWritten =
            LBAwrite(currentDirectoryPtr, parentNumOfBlocks, parentLocation);

        // Testing if we wrote
        struct directoryInfo *parentReadbuf =
            (struct directoryInfo *)malloc(MINBLOCKSIZE);  // For read testing
        int parentRead =
            LBAread(parentReadbuf, parentNumOfBlocks, parentLocation);

        // Write the new directory to memory
        int dirWritten = LBAwrite(newDirectory, dirNumOfBlocks, positionInLBA);
        struct directoryInfo *newDirReadbuf =
            (struct directoryInfo *)malloc(MINBLOCKSIZE);  // For read testing
        int newDirBytesRead =
            LBAread(newDirReadbuf, dirNumOfBlocks, positionInLBA);

        // Update bitmap
        allocate_blocks(map, positionInLBA, dirNumOfBlocks);
        int bitMapBlocks = 5;
        int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
        if (mapBlocksWrote != bitMapBlocks)
            printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
                   bitMapBlocks, mapBlocksWrote);

        free(newDirectory);
        return 0;
    } else {
        printf("This dir already exists! \n");
        return -1;
    }
}

/**
 * @brief Get the Directory struct of the given directory name
 * 
 * @param directoryName Directory name to search for
 * @return struct directoryInfo* Directory struct of the directory
 */
struct directoryInfo *getDirectory(char *directoryName) {
    uint64_t *inode = currentDirectoryPtr->inodes;

    for (int i = 0; i < 50; i++) {
        uint64_t dirEntryStartLocation = inode[i];
        if (dirEntryStartLocation == 0) {
            continue;
        }

        struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
        LBAread(tempDirectory, 1, dirEntryStartLocation);

        // check if file name matches
        if (strcmp(tempDirectory->dirent.fileName, directoryName) == 0) {
            // return nothing if it's a directory
            if (tempDirectory->dirent.fileType == 0) {
                printf(
                    "ERROR: %s is not a directory but a file. Please provide a "
                    "valid "
                    "direcotry name.\n",
                    directoryName);
                free(tempDirectory);
                return NULL;
            }
            // return a pointer to file directory entry
            return tempDirectory;
        }
        // Free tempDir at the end of for loop
        free(tempDirectory);
    }

    printf(
        "ERROR: File with the name you are looking for isn't there. Please "
        "check the name you entered was correct.\n");
    return NULL;
}

/**
 * @brief Get the File struct of the given file name
 * 
 * @param fileName File name to search for
 * @return struct directoryInfo* File struct of the file
 */
struct directoryInfo *getFile(char *fileName) {
    uint64_t *inode = currentDirectoryPtr->inodes;

    for (int i = 0; i < 50; i++) {
        uint64_t dirEntryStartLocation = inode[i];
        if (dirEntryStartLocation == 0) {
            continue;
        }

        struct directoryInfo *tempFile = malloc(MINBLOCKSIZE);
        LBAread(tempFile, 1, dirEntryStartLocation);

        // check if file name matches
        if (strcmp(tempFile->dirent.fileName, fileName) == 0) {
            // return nothing if it's a directory
            if (tempFile->dirent.fileType == 1) {
                printf(
                    "%s is not a file but a directory. Please provide a valid "
                    "file name.\n",
                    fileName);
                free(tempFile);
                return NULL;
            }
            // return a pointer to file directory entry
            return tempFile;
        }
        // Free tempDir at the end of for loop
        free(tempFile);
    }

    printf(
        "Error: File with the name you are looking for isn't there. Please "
        "check the name you entered was correct.\n");
    return NULL;
}

/**
 * @brief Makes a file
 * 
 * @param pathname Pathname of the file
 * @param parentDirectoryInfo Directory struct of the parent directory
 * @param neededBlocks How many blocks needed for the file
 * @param lbaStartPosition Start position of the file in the LBA
 * @return int 0 for success
 */
int fs_mkFile(const char *pathname, struct directoryInfo *parentDirectoryInfo,
              uint64_t neededBlocks, uint64_t lbaStartPosition) {
    if (pathname != NULL) {
        char *path = malloc(strlen(pathname) + 1);
        strcpy(path, pathname);
        char *fileName = getCurrDir(path);

        free(path);

        // Initialize new file struct
        struct directoryInfo *fileDirectoryEntry = malloc(MINBLOCKSIZE);
        strcpy(fileDirectoryEntry->dirent.fileName, fileName);
        fileDirectoryEntry->dirent.blocksForFile = neededBlocks;
        fileDirectoryEntry->dirent.fileSize = 0;
        fileDirectoryEntry->dirent.fileType = 0;  // 0 for file
        strcpy(fileDirectoryEntry->dirent.timeCreated, getCurrTime());
        fileDirectoryEntry->dirent.dirEntStartLocation = lbaStartPosition;

        fileDirectoryEntry->parentDirectoryLocation =
            parentDirectoryInfo->currDirectoryLocation;
        fileDirectoryEntry->currDirectoryLocation = lbaStartPosition;
        fileDirectoryEntry->freeBlock = 0;
        int fileNumOfBlocks = (int)ceil(sizeof(fileDirectoryEntry) / 512.0);
        uint64_t positionInLBA = getNextFreeBlocks(map, fileNumOfBlocks);
        uint64_t *inode = parentDirectoryInfo->inodes;

        // looping to store file position in the available free block
        for (int i = 0; i < 50; i++) {
            uint64_t dirEntStarPosition = inode[i];
            if (dirEntStarPosition == 0) {
                inode[i] = positionInLBA;
                break;
            }
        }
        int dirNumOfBlocks = (int)ceil(sizeof(parentDirectoryInfo) / 512.0);

        // Update the parent directory in memory
        int directoryWritten =
            LBAwrite(parentDirectoryInfo, dirNumOfBlocks,
                     parentDirectoryInfo->dirent.dirEntStartLocation);

        // Write the new file to memory
        int fileWritten =
            LBAwrite(fileDirectoryEntry, fileNumOfBlocks, positionInLBA);

        // Update the bitmap
        allocate_blocks(map, positionInLBA, fileNumOfBlocks);
        int bitMapBlocks = 5;
        int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
        if (mapBlocksWrote != bitMapBlocks)
            printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
                   bitMapBlocks, mapBlocksWrote);
        // printBitMap(map);
        free(fileDirectoryEntry);

        return 0;
    } else {
        printf("Error: Couldn't create a file \n");
        return -1;
    }
}

/**
 * @brief Renames a file
 * 
 * @param oldFilename Old filename 
 * @param newFilename New filename to rename to
 * @return int 0 for success
 */
int fs_renameFile(char *oldFilename, char *newFilename) {
    // make sure the file names aren't empty
    if (strlen(oldFilename) == 0 || strlen(newFilename) == 0) {
        printf("Error: Missing either old filename/new file name or both. \n");
        return -1;

    } else {
        uint64_t *inode = currentDirectoryPtr->inodes;
        for (int i = 0; i < 50; i++) {
            uint64_t dirEntryStartLocation = inode[i];
            if (dirEntryStartLocation == 0) {
                continue;  // couldn't find the allocated space for the file
            }

            // read info about the file to match its name with the name that we
            // want to rename
            struct directoryInfo *tempFile = malloc(MINBLOCKSIZE);
            LBAread(tempFile, 1, dirEntryStartLocation);

            // if file name matches, then rename the file name
            if (strcmp(tempFile->dirent.fileName, oldFilename) == 0) {
                strcpy(tempFile->dirent.fileName, newFilename);

                // update the new name for the file in the disk
                int dirNumOfBlocks = (int)ceil(sizeof(tempFile) / 512.0);
                int directoryWritten =
                    LBAwrite(tempFile, dirNumOfBlocks, dirEntryStartLocation);
                printf("File written in blocks are %d blocks\n",
                       directoryWritten);

                // Update the bitmap
                allocate_blocks(map, dirEntryStartLocation, dirNumOfBlocks);
                int bitMapBlocks = 5;
                int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
                if (mapBlocksWrote != bitMapBlocks)
                    printf(
                        "ERROR: bit map Blocks Wrote was not %d! Instead was "
                        "%d\n",
                        bitMapBlocks, mapBlocksWrote);
                // printBitMap(map);

                free(tempFile);
                return 0;
            } else {
                printf("Error: No file with such name exists.\n");
                free(tempFile);
                return -1;
            }
        }
    }
}

/**
 * @brief Move the file to a given directory
 * 
 * @param fileName File to move
 * @param directoryName  Directory to move into
 * @return int 0 for success
 */
int fs_mvFile(char *fileName, char *directoryName) {
    struct directoryInfo *fileToMove = getFile(fileName);

    if (fileToMove == NULL) {
        printf("Couldnt find the file you're looking for.\n");
        return -1;
    }

    struct directoryInfo *directoryToMoveInto = getDirectory(directoryName);
    if (directoryToMoveInto == NULL) {
        printf("Couldnt find the directory you're looking for.\n");
        return -1;
    }

    // create the file entry for the file that was moved
    if (fs_mkFile(fileToMove->dirent.fileName, directoryToMoveInto,
                  fileToMove->dirent.blocksForFile,
                  fileToMove->dirent.dirEntStartLocation) == 0) {
        // need to remove file now which was copied
        fs_delete(fileName);
        free(fileToMove);
        free(directoryToMoveInto);
        return 0;
    } else {
        free(fileToMove);
        free(directoryToMoveInto);
        printf("Couldn't create the file that was needed to be moved\n");
        return -1;
    }
}

/**
 * @brief Removes the directory
 * 
 * @param pathname Name of directory to remove
 * @return int 0 for success
 */
int fs_rmdir(const char *pathname) {
    char *path = malloc(strlen(pathname) + 1);
    strcpy(path, pathname);
    char *currentDirectoryName = getCurrDir(path);

    // find position of directory where it was stored
    uint64_t *inode = currentDirectoryPtr->inodes;

    uint64_t dirPosition = -1;
    int numOfBlocks = 0;

    for (int i = 0; i < 50; i++) {
        if (currentDirectoryPtr->inodes[i] != 0) {
            struct directoryInfo *searchBuf = (struct directoryInfo *)malloc(
                MINBLOCKSIZE);  // For read testing

            int searchBlocksRead =
                LBAread(searchBuf, 1,
                        currentDirectoryPtr
                            ->inodes[i]);

            if (strcmp(searchBuf->dirent.fileName, currentDirectoryName) == 0) {
                dirPosition = currentDirectoryPtr->inodes[i];
                numOfBlocks = searchBuf->dirent.blocksForFile;
                currentDirectoryPtr->inodes[i] = 0;
            }
            free(searchBuf);
        }
    }

    if (dirPosition == -1) {
        printf("ERROR: Could not find directory! \n");
        return -1;
    }

    LBAwrite(currentDirectoryPtr, 1,
             currentDirectoryPtr->currDirectoryLocation);

    struct directoryInfo *emptyBuf =
        (struct directoryInfo *)malloc(MINBLOCKSIZE);
    LBAwrite(emptyBuf, numOfBlocks, dirPosition);
    free_blocks(map, dirPosition, numOfBlocks);

    int bitMapBlocks = 5;
    int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
    if (mapBlocksWrote != bitMapBlocks)
        printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
               bitMapBlocks, mapBlocksWrote);
    // printBitMap(map);

    free(emptyBuf);

    return 0;
}

/**
 * @brief Opens the directory
 * 
 * @param name Name of directory to open
 * @return fdDir* fdDir struct to return
 */
fdDir *fs_opendir(const char *name) {
    
    // Save the name
    char *path = malloc(strlen(name) + 1);
    strcpy(path, name);
    char *currentDirectoryName = getCurrDir(path);
    free(path);

    struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
    fdDir *dir = malloc(MINBLOCKSIZE);
    // check if malloc was successful
    if (tempDirectory == NULL || dir == NULL) {
        printf("Couldn't allocate the memory for the directory.\n");
    }

    // get current directory's LBAposition that is to be opened
    uint64_t getBlock = currentDirectoryPtr->currDirectoryLocation;
    if (getBlock <= 0) {
        printf("Directory %s couldn't be opened or doesn't exist.\n",
               currentDirectoryName);
        return NULL;
    } else {
        // get metadata of the directory to be opened
        LBAread(tempDirectory, 1, getBlock);

        if (strcmp(tempDirectory->dirent.fileName, currentDirectoryName) == 0) {
            if (tempDirectory->dirent.fileType == 0) {
                printf(
                    "%s is a file, not a directory. Please give a correct "
                    "direcotry name.\n",
                    currentDirectoryName);
                free(tempDirectory);
                return NULL;
            }
            // copy into struct fs_diriteminfo *di;
            strcpy(dir->d_name, tempDirectory->dirent.fileName);
            dir->directoryStartLocation =
                tempDirectory->dirent.dirEntStartLocation;
            dir->d_reclen = sizeof(fdDir);
            dir->offset = 0;

            return ((fdDir *)dir);
        }
        free(tempDirectory);
        free(dir);
    }
}

/**
 * @brief Read the directory
 * 
 * @param dirp Directory info to read into
 * @return struct fs_diriteminfo* The directory object to return
 */
struct fs_diriteminfo *fs_readdir(fdDir *dirp) {
    // iterate through currdirptr's inodes and return non 0 location
    uint64_t *getInodePosition = currentDirectoryPtr->inodes;

    for (int i = dirp->offset; i < 50; i++) {
        uint64_t dirEntryStartLocation = getInodePosition[i];

        if (dirEntryStartLocation == 0) {
            continue;
        }
        struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
        dirp->offset = i + 1;

        // get metadata of the directory at the position found
        LBAread(tempDirectory, 1, dirEntryStartLocation);

        struct fs_diriteminfo *dirInfo = malloc(MINBLOCKSIZE);
        // check if malloc was successful
        if (dirInfo == NULL) {
            printf("Could not allocate the dirInfo buffer.\n");
            return NULL;
        }

        strcpy(dirInfo->d_name, tempDirectory->dirent.fileName);
        dirInfo->fileType = tempDirectory->dirent.fileType;
        dirInfo->d_reclen = sizeof(struct fs_diriteminfo);

        free(tempDirectory);
        return dirInfo;
    }

    return NULL;
}

/**
 * @brief Close the directory
 * 
 * @param dirp Directory info struct to free
 * @return int 0 for success
 */
int fs_closedir(fdDir *dirp) {
    free(dirp);
    dirp = NULL;
    return 0;
}

/**
 * @brief Gets the current working directory
 * 
 * @param buf Directory name to fill
 * @param size Size of the directory
 * @return char* Current directory name
 */
char *fs_getcwd(char *buf, size_t size) {
    char *fullPath = "";
    char *toAppend;

    // Start at currentDirectoryPtr
    struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
    int tempBlocksRead =
        LBAread(tempDirectory, 1, currentDirectoryPtr->currDirectoryLocation);

    if (tempBlocksRead != 1)
        printf("ERROR: Temp Dir Blocks Read was not %d! Instead was %d\n", 1,
               tempBlocksRead);

    do {
        // append the tempDirectory's name to path
        toAppend = getCurrDir(tempDirectory->dirent.fileName);

        // If fullPath is empty, nothing to append, just assign it to toAppend
        if (strlen(fullPath) == 0) {
            fullPath = toAppend;

            // Else append to the beginning of fullPath
        } else {
            fullPath = appendToStart(fullPath, toAppend);
        }

        // Go to the parent
        tempBlocksRead =
            LBAread(tempDirectory, 1, tempDirectory->parentDirectoryLocation);
        if (tempBlocksRead != 1)
            printf("ERROR: Temp Dir Blocks Read was not %d! Instead was %d\n",
                   1, tempBlocksRead);

        // While parent is not itself (root)
    } while (tempDirectory->currDirectoryLocation !=
             tempDirectory->parentDirectoryLocation);

    // append the tempDirectory's name to path
    toAppend = getCurrDir(tempDirectory->dirent.fileName);

    // If fullPath is empty, nothing to append, just assign it to toAppend
    if (strlen(fullPath) == 0) {
        fullPath = toAppend;

        // Else append to the beginning of fullPath
    } else {
        fullPath = appendToStart(fullPath, toAppend);

        // if no directory name in pathname, print root
        if (strcmp(fullPath, "//") == 0) {
            fullPath = strstr(fullPath, "/") + 1;
        }
    }

    // Cleanup
    free(tempDirectory);

    // Return fullpath
    strcpy(buf, fullPath);
    return buf;
}

uint64_t dotdotPosition = 0;
/**
 * @brief Sets the current working directory
 * 
 * @param buf Directory name to read into
 * @return int 0 for success
 */
int fs_setcwd(char *buf) {
    uint64_t rootPosition = 6;

    // when cd into ~ , get to the root directory
    if (strcmp(buf, "~") == 0) {
        if (currentDirectoryPtr != NULL) {
            free(currentDirectoryPtr);
        }
        currentDirectoryPtr = malloc(MINBLOCKSIZE);

        // update current directory to root directory
        LBAread(currentDirectoryPtr, 1, rootPosition);
        return 0;
    }

    // when cd into .. , get to parent directory if exists
    else if ((strlen(buf) != 0) && strcmp(buf, "..") == 0) {
        if (currentDirectoryPtr != NULL) {
            free(currentDirectoryPtr);
        }

        currentDirectoryPtr = malloc(MINBLOCKSIZE);

        // update current directory to root directory
        if (dotdotPosition == rootPosition) {
            LBAread(currentDirectoryPtr, 1, dotdotPosition);
            return 0;
        }
        dotdotPosition -= 1;

        // update current directory to parent directory
        LBAread(currentDirectoryPtr, 1, dotdotPosition);

        return 0;
    }

    else if ((strlen(buf) != 0) || (strcmp(buf, ".") == 0)) {
        char *currDirectory = getCurrDir(buf);

        if (strcmp(buf, ".") == 0) {
            if (currentDirectoryPtr != NULL) {
                free(currentDirectoryPtr);
            }

            currentDirectoryPtr = malloc(MINBLOCKSIZE);
            // update current directory
            LBAread(currentDirectoryPtr, 1, dotdotPosition);
            return 0;
        }

        // find position of directory where it was stored
        uint64_t *inode = currentDirectoryPtr->inodes;

        uint64_t dirPosition = -1;

        for (int i = 0; i < 50; i++) {
            if (currentDirectoryPtr->inodes[i] != 0) {
                struct directoryInfo *searchBuf =
                    (struct directoryInfo *)malloc(
                        MINBLOCKSIZE);  // For read testing
                int searchBlocksRead = LBAread(
                    searchBuf, 1,
                    currentDirectoryPtr
                        ->inodes[i]);

                if (strcmp(searchBuf->dirent.fileName, currDirectory) == 0) {
                    // Check if the filename is a dir/file
                    if (searchBuf->dirent.fileType == 0) {
                        printf("ERROR: This is not a directory! \n");
                        return -1;
                    }

                    dirPosition = currentDirectoryPtr->inodes[i];
                }
                free(searchBuf);
            }
        }

        if (dirPosition == -1) {
            printf("ERROR: Could not find directory! \n");
            return -1;
        }

        if (currentDirectoryPtr != NULL) {
            free(currentDirectoryPtr);
        }

        currentDirectoryPtr = malloc(MINBLOCKSIZE);
        // update current directory
        LBAread(currentDirectoryPtr, 1, dirPosition);

        // set parents location from inode
        dotdotPosition = dirPosition;

        return 0;

    } else {
        return -1;
    }
}

/**
 * @brief Check if the name is a file
 * 
 * @param path Name of the file
 * @return int 1 if it is a file, 0 otherwise
 */
int fs_isFile(char *path) {
    char *fileName = getCurrDir(path);
    if (strlen(fileName) != 0) {
        struct directoryInfo *tempFile = malloc(MINBLOCKSIZE);
        uint64_t *inode = currentDirectoryPtr->inodes;

        for (int i = 0; i < 50; i++) {
            uint64_t dirEntStarPosition = inode[i];
            if (dirEntStarPosition == 0) {
                continue;
            }
            LBAread(tempFile, 1, dirEntStarPosition);
            // if fileType is 0, it's a file
            if (tempFile->dirent.fileType == 0) {
                return 1;
            }
            free(tempFile);
        }

    } else {
        printf("Error: No such file name exists. \n");
        return 0;
    }
}

/**
 * @brief Check if the name is a dir
 * 
 * @param path Name of the dir
 * @return int 1 if it is a dir, 0 otherwise
 */
int fs_isDir(char *path) {
    char *directoryName = getCurrDir(path);
    if (strlen(directoryName) != 0) {
        struct directoryInfo *tempDirectory = malloc(MINBLOCKSIZE);
        uint64_t *inode = currentDirectoryPtr->inodes;

        for (int i = 0; i < 50; i++) {
            uint64_t dirEntStarPosition = inode[i];
            if (dirEntStarPosition == 0) {
                continue;
            }
            LBAread(tempDirectory, 1, dirEntStarPosition);
            // if fileTYpe is 1, it's a directory
            if (tempDirectory->dirent.fileType == 1) {
                return 1;
            }
            free(tempDirectory);
        }
    } else {
        printf("No such directory exists. \n");
        return 0;
    }
}

/**
 * @brief Removes a file
 * 
 * @param filename Name of file to remove
 * @return int 0 for success
 */
int fs_delete(char *filename) {
    char *fileName = malloc(strlen(filename) + 1);
    strcpy(fileName, filename);

    // find position of file  where it was stored
    uint64_t *inode = currentDirectoryPtr->inodes;

    uint64_t filePosition = -1;
    int numOfBlocks = 0;

    for (int i = 0; i < 50; i++) {
        if (currentDirectoryPtr->inodes[i] != 0) {
            struct directoryInfo *searchBuf = (struct directoryInfo *)malloc(
                MINBLOCKSIZE);  // For read testing

            int searchBlocksRead =
                LBAread(searchBuf, 1,
                        currentDirectoryPtr
                            ->inodes[i]);

            if (strcmp(searchBuf->dirent.fileName, fileName) == 0) {
                filePosition = currentDirectoryPtr->inodes[i];
                numOfBlocks = searchBuf->dirent.blocksForFile;
                currentDirectoryPtr->inodes[i] = 0;
            }
            free(searchBuf);
        }
    }

    if (filePosition == -1) {
        printf("ERROR: Could not find file! \n");
        return -1;
    }

    // Update the parent directory
    LBAwrite(currentDirectoryPtr, 1,
             currentDirectoryPtr->currDirectoryLocation);

    // Write an empty buf to memory to "clear"
    struct directoryInfo *emptyBuf =
        (struct directoryInfo *)malloc(MINBLOCKSIZE);
    LBAwrite(emptyBuf, numOfBlocks + 1, filePosition);

    // Update the bit map
    free_blocks(map, filePosition, numOfBlocks + 1);
    int bitMapBlocks = 5;
    int mapBlocksWrote = LBAwrite(map, bitMapBlocks, 1);
    if (mapBlocksWrote != bitMapBlocks)
        printf("ERROR: bit map Blocks Wrote was not %d! Instead was %d\n",
               bitMapBlocks, mapBlocksWrote);
    // printBitMap(map);

    free(emptyBuf);

    return 0;
}

/**
 * @brief Get the stats of a file
 * 
 * @param path Filename
 * @param buf Buffer to read stats into
 * @return int 0 for success
 */
int fs_stat(const char *path, struct fs_stat *buf) {
    char *dirName = malloc(strlen(path) + 1);
    strcpy(dirName, path);
    char *currentDirectoryName = getCurrDir(dirName);

    struct directoryInfo *file = NULL;

    uint64_t *inode = currentDirectoryPtr->inodes;

    for (int i = 0; i < 50; i++) {
        uint64_t dirEntryStartLocation = inode[i];
        if (dirEntryStartLocation == 0) {
            continue;
        }

        struct directoryInfo *tempFile = malloc(MINBLOCKSIZE);
        LBAread(tempFile, 1, dirEntryStartLocation);

        // check if file name matches
        if (strcmp(tempFile->dirent.fileName, currentDirectoryName) == 0) {
            // return a pointer to file directory entry
            // return tempFile;
            file = tempFile;
        }
    }

    buf->st_size = file->dirent.fileSize;
    buf->st_blksize = 512;
    buf->st_blocks = file->dirent.blocksForFile;
    buf->st_accesstime = (time_t)getCurrTime();
    buf->st_createtime = time(NULL);
    buf->st_modtime = time(NULL);
    free(dirName);

    return 0;
}