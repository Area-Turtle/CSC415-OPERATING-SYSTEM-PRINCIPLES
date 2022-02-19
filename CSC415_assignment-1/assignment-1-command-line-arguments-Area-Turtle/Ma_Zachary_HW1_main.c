/*********************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* Github Name: Area-Turtle
* Project: Ma_Zachary_HW1_main.c, Assignment 1 -Command Line Arguments
*
* File: Ma_Zachary_HW1_main.c
* 
* Description: Thie is the main method for the program that will parse through
* a user input and print it out line by line.

*********************************/

#include <stdio.h>

int main(int argc, char * argv[]){
	
	printf("There are %d arguments on the command line.\n", argc);

	/*Loop statement that takes prints each word in the sentence, using i as the index */
	for (int i =0; i <argc; i++){	
		printf("Argument %d:\t%s\n", i , argv[i]);
	}
	return 0;
}
