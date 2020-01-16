// Piano.c
// This software configures the off-board piano keys
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 3/6/17 
// Last Modified: 3/6/17 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "tm4c123gh6pm.h"
#define	open 0      // no sound 0hz
#define B    2531   // 987.8 Hz
#define A    2841   // 880 Hz
#define G    3189   // 784 Hz


// **************Piano_Init*********************
// Initialize piano key inputs, called once 
// Input: none 
// Output: none
void Piano_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x10;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTE_AMSEL_R &= ~0x07; // DISABLE ANALOG FUNC PE0-2
	GPIO_PORTE_PCTL_R &= ~0x00000FFF; //regular GPIO function
  GPIO_PORTE_DIR_R &= ~0x07;    // SET PE0-2 INPUT
  GPIO_PORTE_AFSEL_R &= ~0x07; // REGULAR FUNC PE0-2
	GPIO_PORTE_DEN_R |= 0x07;    // DIGITAL ENABLE PE0-2
}

// **************Piano_In*********************
// Input from piano key inputs 
// Input: none 
// Output: 0 to 7 depending on keys
// 0x01 is just Key0, 0x02 is just Key1, 0x04 is just Key2
uint32_t Piano_In(void){
	if (GPIO_PORTE_DATA_R==0x01){ return (B);}
	else if (GPIO_PORTE_DATA_R==0x02) {return (A);}
	//else if (GPIO_PORTE_DATA_R==0x03) return BA;
	else if (GPIO_PORTE_DATA_R==0x04) {return (G);}
	/*else if (GPIO_PORTE_DATA_R==0x05) return GB;
	else if (GPIO_PORTE_DATA_R==0x06) return GA;
	else if (GPIO_PORTE_DATA_R==0x07) return GA; */
	else {return (open);}
}
