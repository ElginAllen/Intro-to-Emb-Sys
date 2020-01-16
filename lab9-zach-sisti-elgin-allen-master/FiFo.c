// FiFo.c
// Runs on LM4F120/TM4C123
// Provide functions that implement the Software FiFo Buffer
// Last Modified: 4/10/2017 
// Student names: Zach Sisti Elgin Allen       change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly

#include <stdint.h>
// --UUU-- Declare state variables for FiFo
//        size, buffer, put and get indexes
#define Size 16
int32_t PutI;
int32_t GetI;
uint32_t FIFO [Size];	
int32_t FifoCounter = 0;


// *********** FiFo_Init**********
// Initializes a software FIFO of a
// fixed size and sets up indexes for
// put and get operations
void FiFo_Init() {
	PutI = Size - 1;
	GetI = Size - 1;
}

// *********** FiFo_Put**********
// Adds an element to the FIFO
// Input: Character to be inserted
// Output: 1 for success and 0 for failure
//         failure is when the buffer is full
uint32_t FiFo_Put(char data) {
	if (FifoCounter <= (Size - 1)){
		FIFO[PutI]= data;
		if (PutI!=0){
			PutI--;
		}
		else{
			PutI=Size - 1;
		}
		FifoCounter++;
		return 1; 
		}
	else{
		return 0;
		}
   
}

// *********** FiFo_Get**********
// Gets an element from the FIFO
// Input: Pointer to a character that will get the character read from the buffer
// Output: 1 for success and 0 for failure
//         failure is when the buffer is empty
uint32_t FiFo_Get(char *datapt){
	if (FifoCounter>0){
		*datapt = FIFO[GetI];
		if (GetI!=0){
			GetI--;
		}
		else{
			GetI= Size - 1;;
		}
		FifoCounter--;
		return 1;
	}
	else{
		return 0;
	}
}



