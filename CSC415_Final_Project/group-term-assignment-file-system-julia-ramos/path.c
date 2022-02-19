/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: path.c
 *
 * Description: Implementation file for path string functions
 *
 **************************************************************/

#include <stdio.h>
#include <stdio.h>  // printf()
#include <stdlib.h>
#include <string.h>

/**
 * @brief Get the current directory's name
 *
 * @param myPath Full pathname
 * @return char* The current directory's name
 */
char *getCurrDir(char *myPath) {
    char *path = malloc(strlen(myPath) + 1);
    strcpy(path, myPath);
    if (strstr(path, "/") == NULL) return path;
    char *last = strrchr(path, '/');
    if (strcmp((char *)last + 1, "") == 0) return "/";
    return (char *)last + 1;
}

/**
 * @brief Remove the current directory's name
 *
 * @param myPath Full pathname
 * @return char* The full pathname without the current directory
 */
char *removeCurrDir(char myPath[]) {
    char *path = (char *)malloc(strlen(myPath) + 1);
    strcpy(path, myPath);
    char *temp;
    temp = strrchr(path, '/');
    *temp = '\0';
    return (char *)path;
}

/**
 * @brief Appends a token to the end of a full pathname
 *
 * @param myPath Full pathname
 * @param toAppend Token to append
 * @return char* Full pathname after token was appended
 */
char *appendToEnd(char myPath[], char toAppend[]) {
    char *path = (char *)malloc(strlen(myPath) + strlen(toAppend) + 1);
    strcpy(path, myPath);
    strcat(path, "/");
    strcat(path, toAppend);
    return (char *)path;
}

/**
 * @brief Appends a token to the start of a full pathname
 *
 * @param myPath Full pathname
 * @param toAppend Token to append
 * @return char* Full pathname after token was appended
 */
char *appendToStart(char myPath[], char toAppend[]) {
    char *path = (char *)malloc(strlen(myPath) + strlen(toAppend) + 1);
    strcpy(path, "");
    if (strcmp(toAppend, "/") != 0) strcat(path, toAppend);
    strcat(path, "/");
    strcat(path, myPath);
    return (char *)path;
}
