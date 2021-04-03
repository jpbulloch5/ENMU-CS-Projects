/*
Jacob Bulloch
Eastern New Mexico University
CS 461 Operating System Concepts, Dr. Cheng Wen
Project 2, Exercise 4.21
30 March 2021

Instructions:
Write a multithreaded program that calculates various statistical values
for a list of numbers.  This program will be passed a series of numbers on 
the command lne and will then create three separate worker threads.
One thread will determine the average of the numbers, the second 
will determine the minimum value, and the third will determing the 
maximum value.  For example, suppose your program is passed the integers:

    90 81 78 95 79 72 85

The program will report:

    The average value is 82
    The minimum value is 72
    the maximum value is 95

The variables representing the average, minimum, and maximum values
will be stored globally.  The worker threads will set these values, and the
parent thread will output the values once the workers have exited.  (We 
could obviously expand this program by creating additional threads
that determine other statistical values, such as median and standard 
deviation.)

My extras:
    - test for and catch too few or too many arguments passed and allow
      the user to input from the console if needed.
    - added a thread that finds the mode
    - added a thread that finds the median
    - added a thread that finds the standard deviation
*/

#include <stdio.h>      //for print and scan functions
#include <pthread.h>    //for thread functions
#include <stdlib.h>     //for atoi function (converts to integer)
#include <math.h>       //for floor function
#include <stdbool.h>    //for boolean variables

//////GLOBAL DECLARATIONS//////

//sets the maximum length of the list as a global constant for easy changing
#define MAX_LENGTH 20

//These global variables should not be written to by multiple threads,
//All threads need to read the array and know the list length.
//Each thread will need to write to their respective global variable.
int integerArray[MAX_LENGTH], listLength = 0;
int maximum, minimum, mode, median;

//the average and standard deviation involve division, so it should 
//be allowed to have a decimal
float average, stdDev;

//since the standard deviation calculation requires knowing the average
//and is being calculated in a separate thread, we need a flag to sets
//when the average has been calculated so that we can wait for it.
bool foundAverage = false;

//////ROUTINE DEFINITIONS FOR THREADS//////

/**This routine calculates the average of the values in the array of integers.*/
void *averageRoutine(){
    //this variable is used to tally the sum of the integers in the array
    int sum = 0;
    
    //find the sum of all elements in the integer array
    for(int i = 0; i < listLength; i++){
        sum += integerArray[i];
    }
    
    //calculate the average
    average = sum / (listLength + 0.0); // add 0.0 to prevent integer division
    
    foundAverage = true;
}

/**This routine finds the minimum of the values in the array of integers.*/
void *minimumRoutine(){
    //initialize the minimum value to the first value in the array of integers
    minimum = integerArray[0];
  
    for (int i = 1; i < listLength; i++){
        if (minimum > integerArray[i]){
            minimum = integerArray[i];
        }
    }
}

/**This routine finds the maximum of the values in the array of integers.*/
void *maximumRoutine(){
    maximum = integerArray[0];
  
    for (int i = 1; i < listLength; i++){
        if (maximum < integerArray[i]){
            maximum = integerArray[i];
        }
    }
}

/**This routine finds the mode of the values in the array of integers.*/
void *modeRoutine(){
    //these variables are used to find the mode
    int currentCount = 0;
    int maxCount = currentCount;
    
    //initialize the mode to -1, indicating no mode
    mode = -1;

    //iterate through the list
    for(int i = 0; i < listLength; i++){

        //find the mode by checking to see how many times the current element
        //appears later in the array and then comparing that to the max
        currentCount = 0;
        for(int j = i+1; j < listLength; j++){
            if(integerArray[j] == integerArray[i]) currentCount++;
        }

        //if the new number occurs more often than the previous, update it to the
        //new mode
        if(currentCount > maxCount){
            maxCount = currentCount;
            mode = integerArray[i];
        }
    }
}

/**This routine finds the median of the values in the array of integers.*/
void *medianRoutine(){
    median = integerArray[(listLength-1)/2];
}

/**This thread finds the standard deviation of the values in the array 
 * of integers.
 * 
 *      Formula for Standard Deviation:
 *               _____________
 *      s = _   / E((x - m)^2)
 *           \ /  ------------
 *            V      n - 1 
 *      where...
 *          - s indicates standard deviation
 *          - E indicates summation
 *          - x indicates each value of the array
 *          - m indicates the mean, or average, of the array
 *         - n indicates the number of elements in the array
 */
void *stdDevRoutine(){
    
    //do not attempt the calculation until the found average flag is set
    while(!foundAverage){}
    
    //this variable is used to track the summation portion of the calculation
    //in the for...loop below.
    float numeratorSum = 0;
    
    //calculate E((x - m)^2) where... 
    //  - E indicates summation
    //  - x indicates each value of the array
    //  - m indicates the mean, or average, of the array
    //
    //this will become the numerator for the calulation of
    //the standard deviation.
    for(int i = 0; i < listLength; i++){
        numeratorSum += pow((integerArray[i] - average), 2);
    }
    
    //calculate the standard deviation of the array
    stdDev = sqrt(numeratorSum / (listLength - 1));
}

//////MAIN FUNCTION//////

void main(int argc, char *argv[]){
    
    //this check needs to be separate from the next if...else block because
    //after making the change, it will need to execute the same code in the
    //else section in the block below.
    if(argc > MAX_LENGTH){  //if too many arguments were passed...
    
        // truncate the argument list
        argc = MAX_LENGTH;
        
        //inform the user of the truncation
        printf("Too many arguments; truncated to %d elements.", MAX_LENGTH);
        
    }
    
    if(argc < 2){    //if too few arguments were passed...
        
        //prompt user for a valid list length and collect it
        do{
            printf("How many numbers are in your list? (<=%d) \n", MAX_LENGTH);
            scanf("%d", &listLength);
        } while(listLength > MAX_LENGTH);
        
        //prompt user for each list element and collect them
        printf("Enter each number in your list: \n");
        for(int i = 0; i < listLength; i++){
            scanf ("%d", &integerArray[i]);
        }
    
    } else {            //if a valid number of arguments were passed...
        //set the list lenght to the number of agruments less one for the
        //program name
        listLength = argc - 1;
        
        //fill the integer array with the integer value of the arguments
        for(int i = 0; i < listLength; i++){
            integerArray[i] = atoi(argv[i + 1]);
        }
    }
    
    //a constant for the number of threads to prevent "magic numbers"
    #define NUM_THREADS 6
    
    //prepare three thread objects
    pthread_t workerThreads[NUM_THREADS];
    
    //create and link all threads to their appropriate routines
    //each thread will begin from it's associated routine once created
    pthread_create(&workerThreads[0], NULL, &averageRoutine, NULL);
    pthread_create(&workerThreads[1], NULL, &minimumRoutine, NULL);
    pthread_create(&workerThreads[2], NULL, &maximumRoutine, NULL);
    pthread_create(&workerThreads[3], NULL, &modeRoutine, NULL);
    pthread_create(&workerThreads[4], NULL, &medianRoutine, NULL);
    pthread_create(&workerThreads[5], NULL, &stdDevRoutine, NULL);
    
    //wait for the threads to finish so that the output will be correct
    for(int i = 0; i < NUM_THREADS; i++){
        pthread_join(workerThreads[i], NULL);
    }
    
    //output the results to the console
    printf("The average value is:\t%f\t", average);
    printf("The std dev value is:\t%f\n", stdDev);
    printf("the median value is:\t%d\t\t", median);
    
    //there is a special case here, since it is possible for a set of numbers
    //to have no mode, therefore, if the mode is -1, indicating no mode, we
    //will output N/A instead, so it will be more clear to the user.
    //I used the trinary operator (condition) ? (if true) : (if false) to
    //format the output.
    printf("The mode value is:\t%s\n", ((mode > 0) ? "" + mode : "N/A, no mode"));
    printf("The minimum value is:\t%d\t\t", minimum);
    printf("The maximum value is:\t%d\n", maximum);
}
