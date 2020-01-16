// IO.c
// This software configures the switch and LED
// You are allowed to use any switch and any LED, 
// although the Lab suggests the SW1 switch PF4 and Red LED PF1
// Runs on LM4F120 or TM4C123
// Program written by: put your names here
// Date Created: 
// Last Modified:  
// Lab number: 6


#include "tm4c123gh6pm.h"
#include <stdint.h>

//------------IO_Init------------
// Initialize GPIO Port for a switch and an LED
// Input: none
// Output: none
void IO_Init(void) {
		uint32_t delay;
		SYSCTL_RCGCGPIO_R |=0X20;		// set PF4 to switch and PF1 to LED
		for (uint8_t i =10; i>0; i--){
		delay = SYSCTL_RCGCGPIO_R;
		}
		GPIO_PORTF_LOCK_R = 0x4C4F434B;
		GPIO_PORTF_AMSEL_R &= ~0x12;
		GPIO_PORTF_DIR_R |= 0x02;
		GPIO_PORTF_DIR_R &= ~0x10;
		GPIO_PORTF_PUR_R |= 0x10;
		GPIO_PORTF_AFSEL_R &= ~0x12;
		GPIO_PORTF_DEN_R |= 0x12;
}

//------------IO_HeartBeat------------
// Toggle the output state of the  LED.
// Input: none
// Output: none
void IO_HeartBeat(void) {
	GPIO_PORTF_DATA_R^=0x02;
}


//------------IO_Touch------------
// wait for release and press of the switch
// Delay to debounce the switch
// Input: none
// Output: none
void IO_Touch(void) {
		uint32_t debounce=1;// --UUU-- wait for release; delay for 20ms; and then wait for press
		while (debounce>0){
				debounce = GPIO_PORTF_DATA_R&0x10;
		}
		int32_t wait= 320149; //wait 20 ms
		while(wait>0){
		wait--;
		}
		while (debounce==0){
				debounce = GPIO_PORTF_DATA_R&0x10;
		} 
}  

