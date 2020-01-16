// ****************** Lab1.c ***************
// Program written by: Elgin Allen
// Date Created: 1/18/2017 
// Last Modified: 2/07/2017 
// Brief description of the Lab
// An embedded system is capturing temperature data from a
// sensor and performing analysis on the captured data.
// The controller part of the system is periodically capturing N
// readings of the temperature sensor. Your task is to write three
// analysis routines to help the controller perform its function
//   The three analysis subroutines are:
//    1. Calculate the mean of the temperature readings 
//       rounded down to the nearest integer
//    2. Calculate the range of the temperature readings, 
//       defined as the difference between the largest 
//       and smallest reading
//    3. Check if the captured readings are a non-increasing montonic series
//       This simply means that the readings are sorted in non-increasing order.
//       We do not say "increasing" because it is possible for consecutive values
//       to be the same, hence the term "non-increasing". The controller performs 
//       some remedial operation and the desired effect of the operation is to 
//       lower the the temperature of the sensed system. This routine helps 
//       verify whether this has indeed happened
#include <stdint.h>
#define True 1
#define False 0
#define N 21       // Number of temperature readings
uint8_t Readings[N]; // Array of temperature readings to perform analysis on 

// Return the computed Mean 
uint8_t Find_Mean(){
	uint16_t sum = 0;	//holds sum, initialized to 0, needs to be 16bit
	uint8_t mean;		//holds mean 
	uint8_t i = 0;	//Array counter
	while (i < N){	
		sum = sum + Readings[i];	//adds current array value to sum
		i++;											// increments counter to next array value
}
	mean = sum/N;							// usual way of calculating mean
  return(mean);
}
// Return the computed Range
uint8_t Find_Range(){
	uint8_t min = Readings[0];	//initialized as first value in array
	uint8_t max = Readings[0];	//initialized as first value in array
	uint8_t range;							//holds final range
	for (uint8_t i = 0; i < N; i++){
	
		if (Readings[i] < min) min = Readings[i];	//sees if current value in array is < value in min, if so it puts array value into min
		if (Readings[i] > max) max = Readings[i]; //sees if current value in array is > value in max, if so it puts array value into max
	}
	range = max-min;			//usual way of calculating range
  return(range);
}

// Return True of False based on whether the readings
// a non-increasing montonic series
uint8_t IsMonotonic(){
	uint8_t firstArray = 0;		//will hold the first former/first value in array (array [n])
	uint8_t secondArray = 0;	//will hold second/former value in array (array [n+1])
	uint8_t i = 0;						//array counter
	uint8_t returnValue = True; //intialize return value as true to make if else loop work
	while ( i < 21){
		if (firstArray >= secondArray){		//if the scores are monotonic, the else statement will never be branched to, therefore the returnValue will stay True
			firstArray = Readings[i];		// loads array [n]
			i++;												// array [n+1]
			secondArray = Readings[i];	//loads array [n+1]
		}
		else
		{
			i++;								//increments counter inorder to exit while loop
			returnValue = False;	//even if while loop loops back to the first if statement, the return value is still set to False, therefore it will return non-monotonic regardless
		}
}
  return(returnValue);
}



//Testcase 0:
// Scores[N] = {80,75,73,72,90,95,65,54,89,45,60,75,72,78,90,94,85,100,54,98,75};
// Range=55 Mean=77 IsMonotonic=False
//Testcase 1:
// Scores[N] = {100,98,95,94,90,90,89,85,80,78,75,75,75,73,72,72,65,60,54,54,45};	
// Range=55 Mean=77 IsMonotonic=True
//Testcase 2:
// Scores[N] = {80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80};
// Mean=80 Range=0 IsMonotonic=True
//Testcase 3:
// Scores[N] = {100,80,40,100,80,40,100,80,40,100,80,40,100,80,40,100,80,40,100,80,40};
// Mean=73 Range=60 IsMonotonic=False
//Testcase 4:
// Scores[N] = {100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0};
// Range=100 Mean=50 IsMonotonic=True

