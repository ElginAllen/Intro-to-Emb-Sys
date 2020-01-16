// UART1.c
// Runs on LM4F120/TM4C123
// Use UART1 to implement bidirectional data transfer to and from 
// another microcontroller in Lab 9.  This time, interrupts and FIFOs
// are used.
// Daniel Valvano
// November 17, 2014
// Modified by EE345L students Charlie Gough && Matt Hawk
// Modified by EE345M students Agustinus Darmawan && Mingjie Qiu

/* Lab solution, Do not post
 http://users.ece.utexas.edu/~valvano/
*/

// U1Rx (VCP receive) connected to PC4
// U1Tx (VCP transmit) connected to PC5
#include <stdint.h>
#include "FiFo.h"
#include "UART.h"
#include "tm4c123gh6pm.h"

//uint32_t DataLost;
uint32_t RxCounter = 0;
// Initialize UART1
// Baud rate is 115200 bits/sec
// Make sure to turn ON UART1 Receiver Interrupt (Interrupt 6 in NVIC)
// Write UART1_Handler
void UART_Init(void){
	unsigned long delay;
	RxCounter = 0;	//clear global error count
	FiFo_Init(); // yuh
	
	SYSCTL_RCGCUART_R |= 0x0002;	//activate UART 1
	delay++;
	delay++;
	SYSCTL_RCGCGPIO_R |= 0x0004; 	//Activtae PortC
	delay++;
	delay++;
	UART1_CTL_R &= ~0x0001;      	//disable UART
	UART1_IBRD_R = 43; 				// IBRD=int(80000000/(16*115,200)) = int(43.40277)
	UART1_FBRD_R = 26;  				// FBRD = round(0.1267 * 64) = 8
	UART1_LCRH_R = 0x0070;  // 8-bit length, enable FIFO
	UART1_IFLS_R &=~0x38;		// clear bits 5,4,3
	UART1_IFLS_R |= 0x10;		// set bit 4 to 1 (010)
	UART1_IM_R |= 0x10;
  UART1_CTL_R |= 0x0301;   // enable RXE, TXE and UART **POSSIBLE BUG***
	NVIC_PRI1_R |= 0xE00000;		//set bits 21,22,23
	NVIC_EN0_R = 0x40;		//NVIC_EN0_INT6;
	
	GPIO_PORTC_AFSEL_R |= 0x30; // alt funct on PC4,5
  GPIO_PORTC_PCTL_R = (GPIO_PORTC_PCTL_R&0xFF00FFFF)+0x00220000;
  GPIO_PORTC_DEN_R |= 0x30;   // digital I/O on PB1-0
  GPIO_PORTC_AMSEL_R &= ~0x30; // No analog on PB1-0	
}

// input ASCII character from UART
// spin if RxFifo is empty
char UART_InChar(void){
	int32_t useless = 0;
	while((UART1_FR_R&0x10)!=0){
		useless++;
	}
	return ((char)(UART1_DR_R&0xFF)); 
}
//------------UART1_OutChar------------
// Output 8-bit to serial port
// Input: letter is an 8-bit ASCII character to be transferred
// Output: none
void UART_OutChar(char data){
  int32_t useless = 0;
	while((UART1_FR_R&0x20)!=0){
		useless++;
	}
	UART1_DR_R=data;
}

// hardware RX FIFO goes from 7 to 8 or more items
// UART receiver Interrupt is triggered; This is the ISR
void UART1_Handler(void){
	GPIO_PORTF_DATA_R ^= 0x4;		//1. heartbeat toggle
	GPIO_PORTF_DATA_R ^= 0x4;		//2. heartbeat toggle
	while ((UART1_FR_R&0x10)==0){	//3. poll until RXFE = 1
		FiFo_Put(UART_InChar());	//3.abc
		RxCounter++;								//4. increment counter
	}			

	UART1_ICR_R = 0x10;   //5. this clears bit 4 (RXRIS) in the RIS register
	GPIO_PORTF_DATA_R ^= 0x4;		//6. heartbeat toggle	
}
