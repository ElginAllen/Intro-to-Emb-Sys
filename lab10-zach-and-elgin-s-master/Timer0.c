// Timer0.c
// Runs on LM4F120/TM4C123
// Use TIMER0 in 32-bit periodic mode to request interrupts at a periodic rate
// Daniel Valvano
// Last Modified: 3/6/2015 
// You can use this timer only if you learn how it works

/* This example accompanies the book
   "Embedded Systems: Real Time Interfacing to Arm Cortex M Microcontrollers",
   ISBN: 978-1463590154, Jonathan Valvano, copyright (c) 2015
  Program 7.5, example 7.6

 Copyright 2015 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */
#include <stdint.h>

#include "tm4c123gh6pm.h"
#include "Random.h"
#include "ST7735.h"
#include "Images.h"
uint8_t yCoordGlobal;
uint8_t oneortwo;
// ***************** Timer0_Init ****************
// Activate TIMER0 interrupts to run user task periodically

void Timer0_Init(void){
  SYSCTL_RCGCTIMER_R |= 0x01;   // 0) activate TIMER0
  //PeriodicTask0 = task;          // user function
  TIMER0_CTL_R = 0x00000000;    // 1) disable TIMER0A during setup
  TIMER0_CFG_R = 0x00000000;    // 2) configure for 32-bit mode
  TIMER0_TAMR_R = 0x00000002;   // 3) configure for periodic mode, default down-count settings
	uint8_t dumb= (((Random32()>>24)%60)+30);
  TIMER0_TAILR_R =80000000*dumb; // 4) reload value
  TIMER0_TAPR_R = 0;            // 5) bus clock resolution
  TIMER0_ICR_R = 0x00000001;    // 6) clear TIMER0A timeout flag
  TIMER0_IMR_R = 0x00000001;    // 7) arm timeout interrupt
  NVIC_PRI4_R = (NVIC_PRI4_R&0x00FFFFFF)|0x40000000; // 8) priority 4
// interrupts enabled in the main program after all devices initialized
// vector number 35, interrupt number 19
  NVIC_EN0_R = 1<<19;           // 9) enable IRQ 19 in NVIC
  TIMER0_CTL_R = 0x00000001;    // 10) enable TIMER0A
	
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x04;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTC_AMSEL_R &= ~0x50;	//DISABLE ANALOG FUNC PC4,6
	GPIO_PORTC_PCTL_R &= ~0x0F0F0000; // regular GPIO function
	GPIO_PORTC_DIR_R |= 0x50;   // SET PC4,6 OUTPUT
  GPIO_PORTC_AFSEL_R &= ~0x50; // REGULAR FUNC PC4,6
  GPIO_PORTC_DEN_R |= 0x50;    //  DIGITAL ENABLE PC4,6
}

void Timer0A_Handler(void){
	ST7735_DrawBitmap(8, yCoordGlobal, greyBox, 25,12);
  TIMER0_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
	TIMER0_TAILR_R = 80000000*(((Random32()>>24)%60)+30);
	oneortwo= (((Random32()>>24)%2)+1);
	if (oneortwo==2){
		GPIO_PORTC_DATA_R |= 0x40;
	}
	GPIO_PORTC_DATA_R |= 0x10;
	while ((GPIO_PORTE_DATA_R&0x02)==0);
	if (oneortwo==1){
		GPIO_PORTC_DATA_R &= ~0x10;
		
	}
	else if (oneortwo==2){
		while ((GPIO_PORTE_DATA_R&0x02)!=0);
		while ((GPIO_PORTE_DATA_R&0x02)==0);
		GPIO_PORTC_DATA_R &= ~0x50;
	}
}
