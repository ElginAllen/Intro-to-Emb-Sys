// SpaceInvaders.c
// Runs on LM4F120/TM4C123
// Zach Sisti and Elgin Allen
// ******* Possible Hardware I/O connections*******************
//
//ADC_Init makes PE0,PE1 Buttons; PE2 switch; PC4, PC6 LEDs
//
//


// Backlight (pin 10) connected to +3.3 V
// MISO (pin 9) unconnected
// SCK (pin 8) connected to PA2 (SSI0Clk)
// MOSI (pin 7) connected to PA5 (SSI0Tx)
// TFT_CS (pin 6) connected to PA3 (SSI0Fss)
// CARD_CS (pin 5) unconnected
// Data/Command (pin 4) connected to PA6 (GPIO), high for data, low for command
// RESET (pin 3) connected to PA7 (GPIO)
// VCC (pin 2) connected to +3.3 V
// Gnd (pin 1) connected to ground

#include <stdint.h>
#include "tm4c123gh6pm.h"
#include "ST7735.h"
#include "Random.h"
#include "TExaS.h"
#include "ADC.h"
#include "DAC.h"
#include "Sound.h"
#include "Print.h"
#include "Timer1.h"
#include "Images.h"
#include "Timer3.h"
#include "Timer0.h"
void DisableInterrupts(void); // Disable interrupts
void EnableInterrupts(void);  // Enable interrupts


int a;
int b;
uint16_t mistakes = 68;
uint16_t morseLevel = 0;
uint16_t morseCombo;
uint16_t morseInit;
uint32_t answerMorse;

uint16_t bignum;								//Memory globals
uint16_t memArray[4]={0,0,0,0};
uint8_t correctPos;
uint8_t indicatorPos=64;
uint8_t memLoop=0;
//ADC_Init makes PE0,PE1 Buttons; PE2 switch;


void SysTick_Init(void){
  NVIC_ST_CTRL_R = 0;         // disable SysTick during setup
  NVIC_ST_RELOAD_R = 2000000;// reload value
  NVIC_ST_CURRENT_R = 0;      // any write to current clears it
  NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R&0x00FFFFFF)|0x20000000; // priority 1
  NVIC_ST_CTRL_R = 0x0007; // enable SysTick with core clock and interrupts
}




void SysTick_Handler(void){ // every 25 ms
	DisableInterrupts();
	Index = (Index+1)&0x3F;      // 4,5,6,7,7,7,6,5,4,3,2,1,1,1,2,3,... 
  DAC_Out(SineWave[Index]);    // output one value each interrupt
	EnableInterrupts();
}


void MemSet(void){
	uint16_t memInit;
	uint16_t memInput;
	uint16_t yCoord;
	uint16_t completedMem;
	memArray[0]=0, memArray[1]=0,memArray[2]=0, memArray[3]=0;
	
	
	yCoord=40;
	
	
	for(int i=0; i<4;i++){	//initialize memory
		memInit = ((Random32()>>24)%4)+1; // a number from 1 to 4
		while ((memInit==memArray[0])||(memInit==memArray[1])||(memInit==memArray[2])||(memInit==memArray[3])){
			memInit = ((Random32()>>24)%4)+1; // a number from 1 to 4
		}
		if(memInit==1){
			ST7735_DrawBitmap(36, yCoord, mem1, 22,22);
			memArray[i]=memInit;
		}
		else if(memInit==2){
			ST7735_DrawBitmap(36, yCoord, mem2, 22,22);
			memArray[i]=memInit;
		}
		else if(memInit==3){
			ST7735_DrawBitmap(36, yCoord, mem3, 22,22);
			memArray[i]=memInit;
		}
		else if(memInit==4){
			ST7735_DrawBitmap(36, yCoord, mem4, 22,22);
			memArray[i]=memInit;
		}
		yCoord+=25;	
	}
	bignum = ((Random32()>>24)%4)+1; // a number from 0 to 9
	if(bignum==1){
			ST7735_DrawBitmap(62,90, membig1, 52,72);
	}
	else if(bignum==2){
			ST7735_DrawBitmap(62,90, membig2, 52,72);
	}
	else if(bignum==3){
			ST7735_DrawBitmap(62,90, membig3, 52,72);
	}
	else if(bignum==4){
			ST7735_DrawBitmap(62,90, membig4, 52,72);;
	}
}


uint8_t MemStage(void){
	uint8_t completedMem=0;
	uint8_t yCoord;
	uint8_t pos;
		while ((GPIO_PORTE_DATA_R&0x1)==0){	//poll until PE0/button is pressed
			yCoord=ADC_In()/50;
			yCoord+=33;
			yCoordGlobal=yCoord;
			DisableInterrupts();
			ST7735_DrawBitmap(8, yCoord, Selector, 25,12);
			EnableInterrupts();
		}
		while ((GPIO_PORTE_DATA_R&0x1)==1){
		}
		if((yCoord>=18)&&(yCoord<=40)){
			pos=1;
		}
		
		
		else if((yCoord>=43)&&(yCoord<=65)){
			pos=2;
		}
		else if((yCoord>=68)&&(yCoord<=90)){
			pos=3;
		}
		else if((yCoord>=93)&&(yCoord<=115)){
			pos=4;
		}
		if(correctPos==pos)
		{
			completedMem++;
			ST7735_DrawBitmap(indicatorPos, 115, memindicator, 7,22);
			indicatorPos+=9;
			return 1;
		}
		else{
			ST7735_DrawBitmap(mistakes, 158, x, 16,25);
			mistakes+=17;
			if (mistakes==119){
				gameOver=1;
			}
			return 0;
		}
}
void Sound_Morse(void){
	NVIC_ST_RELOAD_R = 2841;
};
void Dot(void){
	//Delay1ms(500);
	Sound_Morse();
	for(int i =0xFFFFF; i>0;i--){

	}
	NVIC_ST_RELOAD_R = 0;
}
void Dash(void){
	Sound_Morse();
	for(int i =0x3FFFFF; i>0;i--){

	}
	NVIC_ST_RELOAD_R = 0;
}
void PlayA(void){
	Dot();
	Delay1ms(1000);
	Dash();
}
void PlayB(void){
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayC(void){
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayE(void){
	Dot();
	Delay1ms(1000);
}
void PlayF(void){
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayH(void){
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayI(void){
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayK(void){
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
}
void PlayL(void){
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayM(void){
	Dash();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
}
void PlayO(void){
	Dash();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
}
void PlayR(void){
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayS(void){
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
}
void PlayT(void){
	Dash();
	Delay1ms(1000);
}
void PlayV(void){
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
}
void PlayX(void){
	Dash();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dot();
	Delay1ms(1000);
	Dash();
	Delay1ms(1000);
}


///
void WiresLevel1(void){
	uint16_t wireInit;
	uint16_t wireInput;
	uint16_t wireArray[4]={0,0,0,0};
	uint16_t yCoord;
	uint16_t completedWires;
	uint16_t blueWires;
	uint16_t redWires;
	uint16_t yellowWires;
	yCoord=33;
	
	for(int i=0; i<4;i++){	//initialize wires
		wireInit = ((Random32()>>24)%6)+1; // a number from 1 to 6
		if(wireInit==1){
			ST7735_DrawBitmap(33, yCoord, BlackLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, BlackRight, 42,13);
			wireArray[i]=wireInit;
		}
		if(wireInit==2){
			ST7735_DrawBitmap(33, yCoord, BlueLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, BlueRight, 42,13);	
			wireArray[i]=wireInit;
		}
		if(wireInit==3){
			ST7735_DrawBitmap(33, yCoord, GreenLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, GreenRight, 42,13);	
			wireArray[i]=wireInit;
		}
		if(wireInit==4){
			ST7735_DrawBitmap(33, yCoord, RedLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, RedRight, 42,13);	
			wireArray[i]=wireInit;
		}
		if(wireInit==5){
			ST7735_DrawBitmap(33, yCoord, WhiteLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, WhiteRight, 42,13);
			wireArray[i]=wireInit;			
		}
		if(wireInit==6){
			ST7735_DrawBitmap(33, yCoord, YellowLeft, 42,13);
			ST7735_DrawBitmap(75, yCoord, YellowRight, 42,13);
			wireArray[i]=wireInit;		
			
	}
		yCoord+=22;		

	}	
	while (completedWires!=1){
		while ((GPIO_PORTE_DATA_R&0x1)==0){	//poll until PE0/button is pressed
			wireInput=ADC_In();
			yCoord=ADC_In()/50;
			yCoord+=33;
			yCoordGlobal=yCoord;
			DisableInterrupts();
			ST7735_DrawBitmap(8, yCoord, scissorsOpen, 25,12);		//moves scissors
			EnableInterrupts();
		}
		while((GPIO_PORTE_DATA_R&0x1)==1);	//wait for button depress
		ST7735_DrawBitmap(8, yCoord, scissorsClosed, 25,12);		//close scissors
		Delay1ms(2000);
		redWires = 0;
		blueWires = 0;					//clear variables
		yellowWires = 0;
		for(int i =0;i<4;i++){					//counts number of blue wires
			if(wireArray[i]==2){
				blueWires++;
			}
		}
		for(int i = 0; i<4;i++){				//counts number of red wires
			if(wireArray[i]==4){
				redWires++;
			}
		}
		for(int i = 0; i<4;i++){				//counts number of yellow wires
			if(wireArray[i]==6){
				yellowWires++;
			}
		}
		
		
		if(	wireArray[3]==6){												//if last wire is yellow, cut first wire
			if(wireInput<=0x250){
				completedWires =1;
			if(wireArray[0]==1){
				ST7735_DrawBitmap(33, 31, BlackLeft, 42,13);
				ST7735_DrawBitmap(75, 35, BlackRight, 42,13);
				Delay1ms(2000);
			}
			if(wireArray[0]==2){
				ST7735_DrawBitmap(33, 31, BlueLeft, 42,13);
				ST7735_DrawBitmap(75, 35, BlueRight, 42,13);
				Delay1ms(2000);				
			}
			if(wireArray[0]==3){
				ST7735_DrawBitmap(33, 31, GreenLeft, 42,13);
				ST7735_DrawBitmap(75, 35, GreenRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[0]==4){
				ST7735_DrawBitmap(33, 31, RedLeft, 42,13);
				ST7735_DrawBitmap(75, 35, RedRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[0]==5){
				ST7735_DrawBitmap(33, 31, WhiteLeft, 42,13);
				ST7735_DrawBitmap(75, 35, WhiteRight, 42,13);	
				Delay1ms(2000);				
			}
			if(wireArray[0]==6){
				ST7735_DrawBitmap(33, 31, YellowLeft, 42,13);
				ST7735_DrawBitmap(75, 35, YellowRight, 42,13);
				Delay1ms(2000);
			}
		}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		else if (blueWires>1){														//if more than one blue wire, cut first wire
			if(wireInput<=0x250){
				completedWires =1;
			if(wireArray[0]==1){
				ST7735_DrawBitmap(33, 31, BlackLeft, 42,13);
				ST7735_DrawBitmap(75, 35, BlackRight, 42,13);
				Delay1ms(2000);
			}
			if(wireArray[0]==2){
				ST7735_DrawBitmap(33, 31, BlueLeft, 42,13);
				ST7735_DrawBitmap(75, 35, BlueRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[0]==3){
				ST7735_DrawBitmap(33, 31, GreenLeft, 42,13);
				ST7735_DrawBitmap(75, 35, GreenRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[0]==4){
				ST7735_DrawBitmap(33, 31, RedLeft, 42,13);
				ST7735_DrawBitmap(75, 35, RedRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[0]==5){
				ST7735_DrawBitmap(33, 31, WhiteLeft, 42,13);
				ST7735_DrawBitmap(75, 35, WhiteRight, 42,13);
					Delay1ms(2000);
			}
			if(wireArray[0]==6){
				ST7735_DrawBitmap(33, 31, YellowLeft, 42,13);
				ST7735_DrawBitmap(75, 35, YellowRight, 42,13);
				Delay1ms(2000);
			}
		}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		else if ((blueWires==1)&&(redWires==1)){					//if 1 blue and 1 red, cut second
			if((wireInput>0x250)&&(wireInput<=0x600)){
				completedWires =1;
			if(wireArray[1]==1){
				ST7735_DrawBitmap(33, 53, BlackLeft, 42,13);
				ST7735_DrawBitmap(75, 57, BlackRight, 42,13);
				Delay1ms(2000);
			}
			if(wireArray[1]==2){
				ST7735_DrawBitmap(33, 53, BlueLeft, 42,13);
				ST7735_DrawBitmap(75, 57, BlueRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[1]==3){
				ST7735_DrawBitmap(33, 53, GreenLeft, 42,13);
				ST7735_DrawBitmap(75, 57, GreenRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[1]==4){
				ST7735_DrawBitmap(33, 53, RedLeft, 42,13);
				ST7735_DrawBitmap(75, 57, RedRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[1]==5){
				ST7735_DrawBitmap(33, 53, WhiteLeft, 42,13);
				ST7735_DrawBitmap(75, 57, WhiteRight, 42,13);	
				Delay1ms(2000);				
			}
			if(wireArray[1]==6){
				ST7735_DrawBitmap(33, 53, YellowLeft, 42,13);
				ST7735_DrawBitmap(75, 57, YellowRight, 42,13);
				Delay1ms(2000);
			}
		}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		
		
		
		else if	(yellowWires>1){									//if more than 2 yellow, cut last
			if(wireInput>0xA00){
				completedWires =1;
			if(wireArray[1]==1){
				ST7735_DrawBitmap(33, 97, BlackLeft, 42,13);
				ST7735_DrawBitmap(75, 101, BlackRight, 42,13);
				Delay1ms(2000);
			}
			if(wireArray[1]==2){
				ST7735_DrawBitmap(33, 97, BlueLeft, 42,13);
				ST7735_DrawBitmap(75, 101, BlueRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[1]==3){
				ST7735_DrawBitmap(33, 97, GreenLeft, 42,13);
				ST7735_DrawBitmap(75, 101, GreenRight, 42,13);
				Delay1ms(2000);				
			}
			if(wireArray[1]==4){
				ST7735_DrawBitmap(33, 97, RedLeft, 42,13);
				ST7735_DrawBitmap(75, 101, RedRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[1]==5){
				ST7735_DrawBitmap(33, 97, WhiteLeft, 42,13);
				ST7735_DrawBitmap(75, 101, WhiteRight, 42,13);
				Delay1ms(2000);				
			}
			if(wireArray[1]==6){
				ST7735_DrawBitmap(33, 97, YellowLeft, 42,13);
				ST7735_DrawBitmap(75, 101, YellowRight, 42,13);
				Delay1ms(2000);
			}
		}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
	}
		else {																		//else cut third wire
			if((wireInput>0x600)&&(wireInput<=0xA00)){
				completedWires =1;
			if(wireArray[2]==1){
				ST7735_DrawBitmap(33, 75, BlackLeft, 42,13);
				ST7735_DrawBitmap(75, 79, BlackRight, 42,13);
				Delay1ms(2000);
			}
			if(wireArray[2]==2){
				ST7735_DrawBitmap(33, 75, BlueLeft, 42,13);
				ST7735_DrawBitmap(75, 79, BlueRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[2]==3){
				ST7735_DrawBitmap(33, 75, GreenLeft, 42,13);
				ST7735_DrawBitmap(75, 79, GreenRight, 42,13);
				Delay1ms(2000);				
			}
			if(wireArray[2]==4){
				ST7735_DrawBitmap(33, 75, RedLeft, 42,13);
				ST7735_DrawBitmap(75, 79, RedRight, 42,13);	
				Delay1ms(2000);
			}
			if(wireArray[2]==5){
				ST7735_DrawBitmap(33, 75, WhiteLeft, 42,13);
				ST7735_DrawBitmap(75, 79, WhiteRight, 42,13);	
				Delay1ms(2000);				
			}
			if(wireArray[2]==6){
				ST7735_DrawBitmap(33, 75, YellowLeft, 42,13);
				ST7735_DrawBitmap(75, 79, YellowRight, 42,13);
				Delay1ms(2000);
			}
		}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		
	}


}


void KeypadLevel1(void){
	uint16_t symbolInit;
	uint16_t symbolArrayInit;
	uint16_t symbolInput;
	uint16_t symbolArray[4];
	symbolArray[0]=0;
	symbolArray[1]=0;
	symbolArray[2]=0;
	symbolArray[3]=0;
	uint16_t symbolCollumn1[6]={4,14,5,1,7,6};
	uint16_t symbolCollumn2[6]={2,6,9,12,7,13};
	uint16_t symbolCollumn3[6]={7,9,18,19,14,12};
	uint16_t symbolCollumn4[6]={11,10,1,18,13,4};
	uint16_t symbolCollumn5[6]={4,10,6,11,17,18};
	uint16_t symbolCollumn6[6]={3,16,15,8,21,20};
	uint16_t alreadyUsed[6]={0,0,0,0,0,0};
	uint16_t yCoord;
	uint16_t counter=0;
	uint16_t completedSymbols;
	yCoord=40;
	symbolArrayInit = ((Random32()>>24)%6)+1; // a number from 1 to 6
	for(int i=0; i<4;i++){
		symbolInit = ((Random32()>>24)%6)+1; // a number from 1 to 6
		while ((symbolInit==alreadyUsed[0])||(symbolInit==alreadyUsed[1])||(symbolInit==alreadyUsed[2])||(symbolInit==alreadyUsed[3])||(symbolInit==alreadyUsed[4])||(symbolInit==alreadyUsed[5])){
			symbolInit = ((Random32()>>24)%6)+1; // a number from 1 to 6
		}
		if (symbolArrayInit==1){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol4, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol14, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol5, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol1, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol7, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol6, 78,22);
				symbolArray[i]=symbolCollumn1[symbolInit-1];
			}
		}
		else if (symbolArrayInit==2){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol2, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol6, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol9, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol12, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol7, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol13, 78,22);
				symbolArray[i]=symbolCollumn2[symbolInit-1];
			}
		}
		else if (symbolArrayInit==3){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol7, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol9, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol18, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol19, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol14, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol12, 78,22);
				symbolArray[i]=symbolCollumn3[symbolInit-1];
			}
		}
		else if (symbolArrayInit==4){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol11, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol10, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol1, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol18, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol13, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol4, 78,22);
				symbolArray[i]=symbolCollumn4[symbolInit-1];
			}
		}
		else if (symbolArrayInit==5){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol4, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol10, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol6, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol11, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol17, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol18, 78,22);
				symbolArray[i]=symbolCollumn5[symbolInit-1];
			}
		}
		else if (symbolArrayInit==6){
			if(symbolInit==1){
				ST7735_DrawBitmap(36, yCoord, Symbol3, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
			else if(symbolInit==2){
				ST7735_DrawBitmap(36, yCoord, Symbol16, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
			else if(symbolInit==3){
				ST7735_DrawBitmap(36, yCoord, Symbol15, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
			else if(symbolInit==4){
				ST7735_DrawBitmap(36, yCoord, Symbol8, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
			else if(symbolInit==5){
				ST7735_DrawBitmap(36, yCoord, Symbol21, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
			else if(symbolInit==6){
				ST7735_DrawBitmap(36, yCoord, Symbol20, 78,22);
				symbolArray[i]=symbolCollumn6[symbolInit-1];
			}
		}
		alreadyUsed[counter]=symbolInit;
		counter++;
		yCoord+=25;
	}	
	uint16_t correctOrder[4];
	correctOrder[0]=0;
	correctOrder[1]=0;
	correctOrder[2]=0;
	correctOrder[3]=0;
	counter=0;
	while (counter<4){
	if (symbolArrayInit==1){
			for (uint8_t k=0; k<6; k++){
				for (uint8_t i=0; i<4; i++){
					if (symbolCollumn1[k]==symbolArray[i]){
						correctOrder[counter]=symbolCollumn1[k];
						counter++;
					}
				}
			}
		}
	else if (symbolArrayInit==2){
			for (uint8_t k=0; k<6; k++){
				for (uint8_t i=0; i<4; i++){
					if (symbolCollumn2[k]==symbolArray[i]){
						correctOrder[counter]=symbolCollumn2[k];
						counter++;
					}
				}
			}
		}
	else if (symbolArrayInit==3){
			for (uint8_t k=0; k<6; k++){
				for (uint8_t i=0; i<4; i++){
					if (symbolCollumn3[k]==symbolArray[i]){
						correctOrder[counter]=symbolCollumn3[k];
						counter++;
					}
				}
			}
		}
	else if (symbolArrayInit==4){
			for (uint8_t k=0; k<6; k++){
				for (uint8_t i=0; i<4; i++){
					if (symbolCollumn4[k]==symbolArray[i]){
						correctOrder[counter]=symbolCollumn4[k];
						counter++;
					}
				}
			}
		}
	else if (symbolArrayInit==5){
			for (uint8_t k=0; k<6; k++){
				for (uint8_t i=0; i<4; i++){
					if (symbolCollumn5[k]==symbolArray[i]){
						correctOrder[counter]=symbolCollumn5[k];
						counter++;
					}
				}
			}
		}
	else if (symbolArrayInit==6){
		for (uint8_t k=0; k<6; k++){
			for (uint8_t i=0; i<4; i++){
				if (symbolCollumn6[k]==symbolArray[i]){
					correctOrder[counter]=symbolCollumn6[k];
					counter++;
				}
			}
		}
	}
}

	while (completedSymbols!=4){
		while ((GPIO_PORTE_DATA_R&0x1)==0){	//poll until PE0/button is pressed
			symbolInput=ADC_In();
			yCoord=ADC_In()/50;
			yCoord+=33;
			yCoordGlobal=yCoord;
			DisableInterrupts();
			ST7735_DrawBitmap(8, yCoord, Selector, 25,12);
			EnableInterrupts();
		}
		while ((GPIO_PORTE_DATA_R&0x1)==1){
		}
		if((yCoord>=18)&&(yCoord<=40)){
			if(symbolArray[0]==correctOrder[completedSymbols])
			{
				completedSymbols++;
				ST7735_DrawBitmap(102, 33, LED, 8,8);
			}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		else if((yCoord>=43)&&(yCoord<=65)){
			if(symbolArray[1]==correctOrder[completedSymbols])
			{
				completedSymbols++;
				ST7735_DrawBitmap(102, 58, LED, 8,8);
			}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			} 
		}
		else if((yCoord>=68)&&(yCoord<=90)){
			if(symbolArray[2]==correctOrder[completedSymbols])
			{
				completedSymbols++;
				ST7735_DrawBitmap(102, 83, LED, 8,8);
			}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
		else if((yCoord>=93)&&(yCoord<=115)){
			if(symbolArray[3]==correctOrder[completedSymbols])
			{
				completedSymbols++;
				ST7735_DrawBitmap(102, 108, LED, 8,8);
			}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
	}
}
uint8_t MemoryLevel1(void){
	uint8_t pos1;
	uint8_t pos2;
	uint8_t pos3;
	uint8_t pos4;
	uint8_t label1;
	uint8_t label2;
	uint8_t label3;
	uint8_t label4;
	ST7735_DrawBitmap(62, 115, membackground, 52,97);
	MemSet();
	if ((bignum==1)||(bignum==2)){
		correctPos=2;
	}
	else if (bignum==3){
		correctPos=3;
	}
	else correctPos=4;
	pos1=correctPos;
	label1=memArray[correctPos-1];
	if (MemStage()==0){
		return 0;
	}
	MemSet();
	if (bignum==1){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==4){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==3){
		correctPos=3;
	}
	else correctPos=pos1;
	pos2=correctPos;
	label2=memArray[correctPos-1];
	if (MemStage()==0){
		return 0;
	}
	MemSet();
	if (bignum==1){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label2){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==2){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label1){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==3){
		correctPos=3;
	}
	else{
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==4){
				correctPos=i+1;
			}
		}
	}
	pos3=correctPos;
	label3=memArray[correctPos-1];
	if (MemStage()==0){
		return 0;
	}
	MemSet();
	if (bignum==1){
		correctPos=pos1;
	}
	else if (bignum==2){
		correctPos=1;
	}
	else correctPos=pos2;
	pos4=correctPos;
	label4=memArray[correctPos-1];
	if (MemStage()==0){
		return 0;
	}
	MemSet();
	if (bignum==1){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label1){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==2){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label2){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==3){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label4){
				correctPos=i+1;
			}
		}
	}
	else if (bignum==4){
		for (uint8_t i=0; i<4; i++){
			if(memArray[i]==label3){
				correctPos=i+1;
			}
		}
	}
	if (MemStage()==0){
		return 0;
	}
	else return 1;
}
void MorseCodeLevel1(void){
	//uint16_t yCoord =33;
	//uint16_t answerMorse;
	morseInit = ((Random32()>>24)%16)+1; // a number from 1 to 16
	ST7735_DrawBitmap(35, 116, MorseSlide, 20,101);
	ST7735_DrawBitmap(70, 71, MorseLCD, 44,12);
	morseLevel = 1;
	while(morseLevel==1){
		if(morseInit==1){//shell	3.505
				PlayS();
				Delay1ms(4000);
				PlayH();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayL();
				Delay1ms(4000);
				PlayL();
				morseCombo=5;
				Delay1ms(10000);
			}
		else if(morseInit==2){//halls	3.515
				PlayH();
				Delay1ms(4000);
				PlayA();
				Delay1ms(4000);
				PlayL();
				Delay1ms(4000);
				PlayL();
				Delay1ms(4000);
				PlayS();
				morseCombo=15;
				Delay1ms(10000);
			}
		else if(morseInit==3){//slick	3.522
				PlayS();
				Delay1ms(4000);
				PlayL();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayK();
				morseCombo=22;
				Delay1ms(10000);
			}
		else if(morseInit==4){//trick	3.532
				PlayT();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayK();
				morseCombo=32;
				Delay1ms(10000);
			}
		else if(morseInit==5){//boxes	3.535
				PlayB();
				Delay1ms(4000);
				PlayO();
				Delay1ms(4000);
				PlayX();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayS();
				morseCombo=35;
				Delay1ms(10000);
			}
		else if(morseInit==6){//leaks	3.542
				PlayL();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayA();
				Delay1ms(4000);
				PlayK();
				Delay1ms(4000);
				PlayS();
				morseCombo=42;
				Delay1ms(10000);
			}
		else if(morseInit==7){//strobe	3.545
				PlayS();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayO();
				Delay1ms(4000);
				PlayB();
				Delay1ms(4000);
				PlayE();
				morseCombo=45;
				Delay1ms(10000);
			}
		else if(morseInit==8){//bistro	3.552
				PlayB();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayS();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayO();
				morseCombo=52;
				Delay1ms(10000);
			}
		else if(morseInit==9){//flick	3.555
				PlayF();
				Delay1ms(4000);
				PlayL();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayK();
				morseCombo=55;
				Delay1ms(10000);
			}
		else if(morseInit==10){//bombs	3.565
				PlayB();
				Delay1ms(4000);
				PlayO();
				Delay1ms(4000);
				PlayM();
				Delay1ms(4000);
				PlayB();
				Delay1ms(4000);
				PlayS();
				morseCombo=65;
				Delay1ms(10000);
			}
		else if(morseInit==11){//break	3.572
				PlayB();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayA();
				Delay1ms(4000);
				PlayK();
				morseCombo=72;
				Delay1ms(10000);
			}
		else if(morseInit==12){//brick	3.575
				PlayB();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayK();
				morseCombo=75;
				Delay1ms(10000);
			}
		else if(morseInit==13){//steak	3.582
				PlayS();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayA();
				Delay1ms(4000);
				PlayK();
				morseCombo=82;
				Delay1ms(10000);
			}
		else if(morseInit==14){//string	3.592
				PlayS();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayR();
				Delay1ms(4000);
				PlayI();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayK();
				morseCombo=92;
				Delay1ms(10000);
			}
		else if(morseInit==15){//vector	3.595	
				PlayV();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayC();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayO();
				Delay1ms(4000);
				PlayR();
				morseCombo=95;
				Delay1ms(10000);
			}
		else if(morseInit==16){//beats	3.600
				PlayB();
				Delay1ms(4000);
				PlayE();
				Delay1ms(4000);
				PlayA();
				Delay1ms(4000);
				PlayT();
				Delay1ms(4000);
				PlayS();
				morseCombo=100;
				Delay1ms(10000);
			}
		//while ((GPIO_PORTE_DATA_R&0x1)==0){	//poll until PE0/button is pressed
			
		//}
		/*while ((GPIO_PORTE_DATA_R&0x1)==1);
		if(answerMorse==morseCombo){
			morseLevel=0;
		}
		else{
			ST7735_DrawBitmap(mistakes, 158, x, 16,25);
			mistakes+=17;
			if (mistakes==102){
					ST7735_FillScreen(0x0000);            // set screen to black
					ST7735_SetCursor(3,3);
					ST7735_OutString("Game Over");
				}
		}*/
	}
}	


void Output_Clock(){
	 if (min==9){
		 ST7735_DrawBitmap(21,151, nine, 8,12);
	 }
	 else if (min==8){
		 ST7735_DrawBitmap(21,151, eight, 8,12);
	 }
	 else if (min==7){
		 ST7735_DrawBitmap(21,151, seven, 8,12);
	 }
	 else if (min==6){
		 ST7735_DrawBitmap(21,151, six, 8,12);
	 }
	 else if (min==5){
		 ST7735_DrawBitmap(21,151, five, 8,12);
	 }
	 else if (min==4){
		 ST7735_DrawBitmap(21,151, four, 8,12);
	 }
	 else if (min==3){
		 ST7735_DrawBitmap(21,151, three, 8,12);
	 }
	 else if (min==2){
		 ST7735_DrawBitmap(21,151, two, 8,12);
	 }
	 else if (min==1){
		 ST7735_DrawBitmap(21,151, one, 8,12);
	 }
	 else if (min==0){
		 ST7735_DrawBitmap(21,151, zero, 8,12);
	 }
	 ST7735_DrawBitmap(30,151, collon, 5,11);
	 if (tensec==9){
		 ST7735_DrawBitmap(35,151, nine, 8,12);
	 }
	 else if (tensec==8){
		 ST7735_DrawBitmap(35,151, eight, 8,12);
	 }
	 else if (tensec==7){
		 ST7735_DrawBitmap(35,151, seven, 8,12);
	 }
	 else if (tensec==6){
		 ST7735_DrawBitmap(35,151, six, 8,12);
	 }
	 else if (tensec==5){
		 ST7735_DrawBitmap(35,151, five, 8,12);
	 }
	 else if (tensec==4){
		 ST7735_DrawBitmap(35,151, four, 8,12);
	 }
	 else if (tensec==3){
		 ST7735_DrawBitmap(35,151, three, 8,12);
	 }
	 else if (tensec==2){
		 ST7735_DrawBitmap(35,151, two, 8,12);
	 }
	 else if (tensec==1){
		 ST7735_DrawBitmap(35,151, one, 8,12);
	 }
	 else if (tensec==0){
		 ST7735_DrawBitmap(35,151, zero, 8,12);
	 }
	 if (sec==9){
		 ST7735_DrawBitmap(44,151, nine, 8,12);
	 }
	 else if (sec==8){
		 ST7735_DrawBitmap(44,151, eight, 8,12);
	 }
	 else if (sec==7){
		 ST7735_DrawBitmap(44,151, seven, 8,12);
	 }
	 else if (sec==6){
		 ST7735_DrawBitmap(44,151, six, 8,12);
	 }
	 else if (sec==5){
		 ST7735_DrawBitmap(44,151, five, 8,12);
	 }
	 else if (sec==4){
		 ST7735_DrawBitmap(44,151, four, 8,12);
	 }
	 else if (sec==3){
		 ST7735_DrawBitmap(44,151, three, 8,12);
	 }
	 else if (sec==2){
		 ST7735_DrawBitmap(44,151, two, 8,12);
	 }
	 else if (sec==1){
		 ST7735_DrawBitmap(44,151, one, 8,12);
	 }
	 else if (sec==0){
		 ST7735_DrawBitmap(44,151, zero, 8,12);
	 }
}
	
void Timer1A_Handler(void){
	if (gameOver==1){
		DisableInterrupts();		
		ST7735_FillScreen(0x0000);            // set screen to black
		ST7735_SetCursor(3,3);
		ST7735_OutString("Game Over");
		while (1);
	}
	TIMER1_ICR_R = TIMER_ICR_TATOCINT;	//acknowledge TIMER1A timeout
	DisableInterrupts();
	Clock_Change();											//execute user task
	EnableInterrupts();
	Output_Clock();
	//check if mistakes==102, if so display gameover screen
} 

void Timer3A_Handler(void){
  TIMER3_ICR_R = TIMER_ICR_TATOCINT;// acknowledge TIMER0A timeout
	if((GPIO_PORTE_DATA_R&0x02)==1){
		if(tensec>=4){
			tensec-=4;
		}
		else {
			if(min!=0){
				min--;
				tensec=6-(4-tensec);
			}
			else {
				gameOver=1;
			}
		}
	}
	if(morseLevel==1){

		uint16_t yCoord;

		answerMorse=ADC_In();
		answerMorse/=39;
		yCoord=ADC_In()/50;
		yCoord+=33;
		yCoordGlobal=yCoord;
		DisableInterrupts();
		ST7735_DrawBitmap(8, yCoord, Selector, 25,12);		//moves scissors
		ST7735_SetCursor(14,6);
		LCD_OutFix(answerMorse+3500);
		EnableInterrupts();
		if ((GPIO_PORTE_DATA_R&0x1)==1){
			while((GPIO_PORTE_DATA_R&0x1)==1);
			if(answerMorse==morseCombo){
				morseLevel=0;
			}
			else{
				ST7735_DrawBitmap(mistakes, 158, x, 16,25);
				mistakes+=17;
				if (mistakes==119){
					gameOver=1;
				}
			}
		}
	}
}
int main(void){
 
	TExaS_Init();	
	DisableInterrupts();
	SysTick_Init();
	ST7735_InitR(INITR_REDTAB); 
	Sound_Init();
	DAC_Init();
	ADC_Init();

	EnableInterrupts();

  Output_Init();
  ST7735_FillScreen(0x0000);            // set screen to black
  ST7735_SetCursor(3,3);
  ST7735_OutString("Press button");
	ST7735_SetCursor(3,4);
	ST7735_OutString("to start");
	NVIC_ST_RELOAD_R = 2000000;
	while((GPIO_PORTE_DATA_R&0x1)==0);
	while((GPIO_PORTE_DATA_R&0x1)==1);
	Random_Init(NVIC_ST_CURRENT_R);

	Timer1_Init();
	Timer0_Init();
	Timer3_Init();
	
	ST7735_DrawBitmap(0, 160, MainScreen, 128,160); //
  ST7735_SetCursor(3,3);
  ST7735_OutString("Level 1");
	Delay1ms(4000);
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
  
	
	WiresLevel1();
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
  ST7735_SetCursor(3,3);
  ST7735_OutString("Level 2");
	Delay1ms(4000);
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
	KeypadLevel1();
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
  ST7735_SetCursor(3,3);
  ST7735_OutString("Level 3");
	Delay1ms(4000);
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
	MorseCodeLevel1();
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
  ST7735_SetCursor(3,3);
  ST7735_OutString("Level 4");
	Delay1ms(4000);
	ST7735_DrawBitmap(0, 132, ResetScreen, 128,132);
	while (memLoop==0){
		memLoop=MemoryLevel1();
	}
	
  while(1){
  DisableInterrupts();
	for(int i=0;i<5;i++){
	ST7735_SetCursor(0,i);
	ST7735_OutString("********************");
	}
	ST7735_SetCursor(0,5);
	ST7735_OutString("****Bomb Defused****");
	}
	for(int i=6;i<10;i++){
	ST7735_SetCursor(0,i);
	ST7735_OutString("********************");
	}
}


