/**************************************************************
* Class:  CSC-415-03 Fall 2020
* Name: Zachary Ma
* Student ID: 916750281
* GitHub ID: Area-Turtle
* Project: Assignment 4 – Word Blast
*
* File: Ma_Zachary_HW4_main
*
* Description: The program is to use threads to parse through a text file and print out the most common words.
*
**************************************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>

#define BUFFER_SIZE 512

// You may find this Useful
char * delim = "\"\'.“”‘’?:;-,—*($%)! \t\n\x0A\r";

void *thread_WordList() 
	{
	//initial variables
	FILE *document;
    	int words = 0;
    	char placeWordFile[BUFFER_SIZE];
    	char placeArray[200000][40];
    	int placeFreq[20000];    
    	int f = 0;
    	int newWord = 0;
    	document = fopen("WarAndPeace.txt","r");
    	if(document == NULL) 
	{
        	printf("Error! Unable to open this file.");
        	exit(0);
    	}
    	else 
	{
        	char* placeRead;
        	// reads file till end
        	while(fscanf(document, "%s", placeWordFile) != EOF) 
		{
			//removes extra bits
            		placeRead = strtok(placeWordFile, delim);

            		while(placeRead != NULL) 
			{
                		//checks characters of at min length of 6
                		if(strlen(placeRead) > 5) 
				{
                    			words++;
                    			//moves along arrary adding words to placeholder string if they are unique
                    			char placeStringArray[strlen(placeRead)];
                    			strcpy(placeStringArray, placeRead);

                   			//if current word doesn't exist in array adds word to array. 
                    			for(int a = 0; a <= f; a++) 
					{
                        			if(a == f) 
						{
                            				memcpy(placeArray[newWord], placeStringArray, strlen(placeRead));
                            				int num = 1;
                            				placeFreq[newWord] = num;
							newWord++;
                        			}

                        			//if current word is the same, adds 1 to the word count
                        			else if(strcasecmp(placeArray[a], placeRead) == 0) 
						{
                            				int placeNum = placeFreq[a];
                            				placeNum++;
                            				placeFreq[a] = placeNum;
                            				break;
                        			}
                    			}
                    			f++;
                		}
                		placeRead = strtok(NULL, delim);
            		}
        	}
        	fclose(document);
	}

    //word sort/print section
    
    	for(int b = 0; b < newWord; b++) 
	{
		int placeholder;
        	for(int c = 0; c < newWord; c++) 
		{
            		if(placeFreq[c] < placeFreq[b]) 
			{
                		//moves larger frequency words to top of list.
                		placeholder = placeFreq[b];
                		placeFreq[b] = placeFreq[c];
	               		placeFreq[c] = placeholder;

                		//combines strings together
                		char placeWord2[BUFFER_SIZE];
                		strcpy(placeWord2, placeArray[b]);
                		strcpy(placeArray[b], placeArray[c]);
                		strcpy(placeArray[c], placeWord2);
            		}
        	}
    	}

    	printf("\n%s\n\n", "\nPrinting top 10 words 6 characters or more:");
    
    	//prints top 10 words for array
    	if(newWord >= 10) 
	{
        	for(int d = 0; d < 10;  d++) 
		{
			printf("Number %d is %s with a count of %d\n",d+1, placeArray[d], placeFreq[d]);
        	}
	}
    return NULL;
}

//thread counter
void* thread_gen1(void* arg) 
	{
    		for(int e = 0; e < arg; e++) 
		{
        		printf("%s - %d\n", "Thread 1", e);
    		}
    	return NULL;
	}


int main (int argc, char *argv[])
    {
    //***TO DO***  Look at arguments, open, divide by threads
    //             Allocate and Initialize and storage structures
	

	int counts;
	printf("Enter amount of threads: ");
	scanf("\n%d", &counts);

	
	

    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Time stamp start
    struct timespec startTime;
    struct timespec endTime;

    clock_gettime(CLOCK_REALTIME, &startTime);
    //**************************************************************
    // *** TO DO ***  start your thread processing
    //                wait for the threads to finish
	pthread_t thread_1;
    	pthread_t thread_2;

    	pthread_t threads[counts];
	

    	char *mage1 = "Thread1";
    	char *mage2 = "Thread2";

	printf("Word Frequency Count of WarAndPeace.txt with %d threads", counts);
    	thread_WordList();
	

    	pthread_create(&thread_2, NULL, thread_gen1, NULL);

    

    

    // ***TO DO *** Process TOP 10 and display


    //**************************************************************
    // DO NOT CHANGE THIS BLOCK
    //Clock output
    clock_gettime(CLOCK_REALTIME, &endTime);
    time_t sec = endTime.tv_sec - startTime.tv_sec;
    long n_sec = endTime.tv_nsec - startTime.tv_nsec;
    if (endTime.tv_nsec < startTime.tv_nsec)
        {
        --sec;
        n_sec = n_sec + 1000000000L;
        }

    printf("Total Time was %ld.%09ld seconds\n", sec, n_sec);
    //**************************************************************

	pthread_exit(NULL);
    // ***TO DO *** cleanup
    }
