/**************************************************************
 * Class:  CSC-415-03
 * Name: Julia Beatriz Ramos, Danish Siddiqui, Zachary Ma, Michael Morales
 * Student ID: 920638040, 920258388, 916750281, 920237211
 * Project: File System Project
 *
 * File: path.h
 *
 * Description: Header file for path string functions
 *
 **************************************************************/
#ifndef _PATH_H
#define _PATH_H

/**
 * @brief Get the current directory's name
 *
 * @param myPath Full pathname
 * @return char* The current directory's name
 */
char *getCurrDir(char *myPath);

/**
 * @brief Remove the current directory's name
 *
 * @param myPath Full pathname
 * @return char* The full pathname without the current directory
 */
char *removeCurrDir(char myPath[]);

/**
 * @brief Appends a token to the end of a full pathname
 *
 * @param myPath Full pathname
 * @param toAppend Token to append
 * @return char* Full pathname after token was appended
 */
char *appendToEnd(char myPath[], char toAppend[]);

/**
 * @brief Appends a token to the start of a full pathname
 *
 * @param myPath Full pathname
 * @param toAppend Token to append
 * @return char* Full pathname after token was appended
 */
char *appendToStart(char myPath[], char toAppend[]);

#endif