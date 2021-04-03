/*
Jacob Bulloch
Eastern New Mexico University
CS 461 Operating System Concepts, Dr. Cheng Wen
Project 1, Exercise 3.21
30 March 2021

Instructions:
The Collatz conjecture concerns what happens when we take any
positive integer n and apply the following algorithm:

	    (
	n = < n / 2	if n is even
	    ( 3 * n + 1 if n is odd
	    
The conjecture states that when this algorithm is continually applied,
all positive integers will eventually reach 1.  For example, if n = 35, the
sequence is:

	35, 106, 53, 160, 80, 40, 20, 10, 5, 16, 8, 4, 2, 1
	
Write a C program using the fork() system call that generates this 
sequence in the child process.  The starting number will be provided 
from the command line.  For example, if 8 is passed as a parameter on
the command line, the child process will output 8, 4, 2, 1.  Because the
parent and child processes have their own copies of the data, it will be 
necessary for the child to output the sequence.  Have the parent invoke 
the wait() call to wait for the child process to complete before exiting 
the program.  Perform necessary error checking to ensure that a positive 
integer is passed on the command line.

My extras:
    - allow user to enter input from the console if no valid arguments 
      were found
*/


#include <stdio.h>      //for print and scan functions
#include <sys/wait.h>   //for wait function
#include <unistd.h>     //for fork function
#include <stdlib.h>     //for atoi function
#include <stdbool.h>    //for boolean variable

void main(int argc, char *argv[]){
    
	//this variable is used to collect and manipulate the user's input.
	int userInput = 0;
	
	//this variable is used to track the fork() operations
	pid_t pid;
	
	//this flag is used to trigger the collection of user input if no valid
	//arguments are found
	bool validArguments = true;
	
	//if the user did not pass an argument via the command line...
	if(argc < 2){
	    printf("Arguments not found!");
	    validArguments = false;
	} else {
	    
	    // take the second argument (not the name of the program)
	    // as the user's input.
	    userInput = atoi(argv[1]);
	    if (userInput <= 0){
	        printf("The argument [%s] is not a positive integer!\n", argv[1]);
	        validArguments = false;
	    }
	}
	
	//if no valid argument was found...
	if(!validArguments){
	    
    	//collect the user's input from the console
    	//here I assume the user will follow directions
	    do{
	    	printf("Enter a positive integer: "); 
  	    	scanf("%d", &userInput);	
	    }while (userInput <= 0);
	}
	
	pid = fork();

	if (pid == 0){  //Child Process:
		
		//display the un-manipulated value the user gave as input
		printf("%d\n", userInput);
		
		//execute the required operation until the manipulated input = 0
		while (userInput != 1){
		    
			if (userInput % 2 == 0){             // if even...
				userInput /= 2;                  // divide by 2
			} else {                             // if odd...
				userInput = (3 * userInput) + 1; // multiply by 3, and add 1
			}	
			
			//display the current manipulated value of the user's input
			printf("%d\n", userInput);
		}
		
		//End of child process
	} else {    //Parent Process:
	
		//wait for the child process to finish
		wait(NULL);
		
		//End of parent process
	}
}
