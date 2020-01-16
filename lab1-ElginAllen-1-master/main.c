// main.c
// Runs on LM3S1968 using the simulator
// Grading engine for Lab1

// U0Rx (VCP receive) connected to PA0
// U0Tx (VCP transmit) connected to PA1
#include <stdint.h>
#define True 1
#define False 0
#define Range 0
#define Mean 1
#define Sorted 2
#include "UART.h"
#define N 21
#define COUNT 5
struct TestCase{
  uint8_t Readings[N];  // Array of readings
  uint8_t Stats[3]; // [0]:Range; [1]:Mean; [2]: Sorted or not
	
};
typedef struct TestCase TestCaseType;
TestCaseType Tests[COUNT]={
  { {80,75,73,72,90,95,65,54,89,45,60,75,72,78,90,94,85,100,54,98,75},{55,77,False}},
	{	{100,98,95,94,90,90,89,85,80,78,75,75,75,73,72,72,65,60,54,54,45},{55,77,True}},	
	{ {80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80,80},{0,80,True}},
	{ {100,80,40,100,80,40,100,80,40,100,80,40,100,80,40,100,80,40,100,80,40},{60,73,False}},
	{ {100,95,90,85,80,75,70,65,60,55,50,45,40,35,30,25,20,15,10,5,0},{100,50,True}}	
};
extern uint8_t Readings[N];
//prototypes of student code
uint8_t Find_Mean(void);      // Return the computed Mean 
uint8_t Find_Range(void);     // Return the computed Range
uint8_t IsMonotonic(void);    // Return True of False based on whether the readings
                          // a non-increasing montonic series
int main(void){ 
  unsigned long n,i;
  uint8_t result,success=True, fails=0;

  UART_Init();              // initialize UART

  UART_OutString("\n\rTemperature Sensor Data Analysis\n\r");
  for(n = 0; n < COUNT ; n++){
    UART_OutString(" Test Case ");UART_OutUDec(n); UART_OutCRLF();
    for (i=0; i<N; i++){
		  Readings[i] = Tests[n].Readings[i];
    }
    result = Find_Mean();
    if(result == Tests[n].Stats[Mean]){
      UART_OutString(" Yes, Your Mean= ");
    }else{
      UART_OutString(" No, Correct Mean= ");
      UART_OutUDec(Tests[n].Stats[Mean]);  
      UART_OutString(" Your Mean= ");
      success=False; fails++;
    }
    UART_OutUDec(result);UART_OutCRLF();
		
    result = Find_Range();
    if(result == Tests[n].Stats[Range]){
      UART_OutString(" Yes, Your Range= ");
    }else{
      UART_OutString(" No, Correct Range= ");
      UART_OutUDec(Tests[n].Stats[Range]);  
      UART_OutString(" Your Range= "); success=False;fails++;
    }
    UART_OutUDec(result);UART_OutCRLF();
		
    result = IsMonotonic();
    if(result == Tests[n].Stats[Sorted]){
      UART_OutString(" Correct Analysis of monotonicity ");
    }else{
      UART_OutString(" Incorrect Analysis of monotonicity ");success=False;fails++;
    }
    UART_OutCRLF();
  }
	if (success) {
    UART_OutString(" Passed all tests - ");
	}else{
    UART_OutString(" Failing ");
    UART_OutUDec(fails);
    UART_OutString("/15 tests");
	}
  UART_OutString(" End of Analysis\n\r");
  while(1){
  }
}
