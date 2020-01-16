// Lab8.c
// Runs on LM4F120 or TM4C123
// Student names: change this to your names or look very silly
// Last modification date: change this to the last modification date or look very silly
// Last Modified: 4/5/2016 

// Analog Input connected to PE2=ADC1
// displays on Sitronox ST7735
// PF3, PF2, PF1 are heartbeats

#include <stdint.h>
#include "ST7735.h"
//#include "TExaS.h"
#include "ADC.h"
#include "print.h"
#include "tm4c123gh6pm.h"
#include "PLL.h"

//*****the first three main programs are for debugging *****
// main1 tests just the ADC and slide pot, use debugger to see data
// main2 adds the LCD to the ADC and slide pot, ADC data is on Nokia
// main3 adds your convert function, position data is no Nokia

void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


#define PF1       (*((volatile uint32_t *)0x40025008))
#define PF2       (*((volatile uint32_t *)0x40025010))
#define PF3       (*((volatile uint32_t *)0x40025020))
uint32_t Data;        // 12-bit ADC
uint32_t Position;    // 32-bit fixed-point 0.001 cm
int32_t ADCMailbox;
int32_t ADCStatus;
// Initialize Port F so PF1, PF2 and PF3 are heartbeats
void PortF_Init(void){
	volatile unsigned long delay;
	SYSCTL_RCGCGPIO_R |= 0x20;
	delay = SYSCTL_RCGCGPIO_R;
	GPIO_PORTF_AMSEL_R &= ~0xE;	//DISABLE ANALOG FUNC PF1-3
	GPIO_PORTF_PCTL_R &= ~0x0000FFF0; // regular GPIO function
	GPIO_PORTF_DIR_R |= 0xE;   // SET PF1-3OUTPUTS 
  GPIO_PORTF_AFSEL_R &= ~0xE; // REGULAR FUNC PF1-3
  GPIO_PORTF_DEN_R |= 0xE;    //  DIGITAL ENABLE PF1-3
}
void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 0x007FFFFF;;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
}
void SysTick_Handler(void){
	PF2 ^= 0x04;  
	PF2 ^= 0x04;  
	ADCMailbox=ADC_In();
	ADCStatus = 1;
	PF2 ^= 0x04;
	
}


int main1(void){      // single step this program and look at Data
  PLL_Init();       // Bus clock is 80 MHz 
  ADC_Init();         // turn on ADC, set channel to 1, PE4
  while(1){                
    Data = ADC_In();  // sample 12-bit channel 1, PE4
  }
}

int main2(void){
  PLL_Init();       // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
	ADC_Init();         // turn on ADC, set channel to 1
  PortF_Init();
  while(1){           // use scope to measure execution time for ADC_In and LCD_OutDec           
    PF2 = 0x04;       // Profile ADC
    Data = ADC_In();  // sample 12-bit channel 1
    PF2 = 0x00;       // end of ADC Profile
    ST7735_SetCursor(0,0);
    PF1 = 0x02;       // Profile LCD
    LCD_OutDec(Data); 
    ST7735_OutString("    ");  // these spaces are used to coverup characters from last output
    PF1 = 0;          // end of LCD Profile
  }
}

uint32_t Convert(uint32_t input){//(1097x+115582)/2325714
	uint32_t result = input;
	result*=1097;
	result+=115;
	result/=2326;
	return result;
}
int main3(void){ 
  PLL_Init();         // Bus clock is 80 MHz 
  ST7735_InitR(INITR_REDTAB); 
  PortF_Init();
  ADC_Init();         // turn on ADC, set channel to 1
  while(1){  
    PF2 ^= 0x04;      // Heartbeat
    Data = ADC_In();  // sample 12-bit channel 1
    PF3 = 0x08;       // Profile Convert
    Position = Convert(Data); 
    PF3 = 0;          // end of Convert Profile
    PF1 = 0x02;       // Profile LCD
    ST7735_SetCursor(0,0);
    LCD_OutDec(Data); ST7735_OutString("    "); 
    ST7735_SetCursor(6,0);
    LCD_OutFix(Position);
    PF1 = 0;          // end of LCD Profile
  }
}  
int main(void){
  PLL_Init();
	PortF_Init();
	DisableInterrupts();
	SysTick_Init();
	ST7735_InitR(INITR_REDTAB); 
	ADC_Init();
	EnableInterrupts();
	while(1){
		
		int32_t ADCHolder;
		while(ADCStatus==0){
			ADCHolder=1;
		}
		ADCHolder = ADCMailbox;
		ADCStatus = 0;
		ST7735_SetCursor(0,0);
		LCD_OutFix(Convert(ADCHolder));
		ST7735_OutString("cm");
  }
}
