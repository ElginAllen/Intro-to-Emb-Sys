;****************** main.s ***************
; Program written by: Elgin Allen Zach Sisti
; Date Created: 2/4/2017
; Last Modified: 2/16/2017
; Brief description of the program
;   The LED toggles at 8 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external9 LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE0 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 8Hz,
;      which is 8 times per second with a duty-cycle of 20%.
;      Therefore, the LED is ON for (0.2*1/8)th of a second
;      and OFF for (0.8*1/8)th of a second.
;   3) When the button on (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 20% to 40% to 60%
;      to 80% to 100%(ON) to 0%(Off) to 20% to 40% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 8Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 20%.
;      TIP: debugging the breathing LED algorithm and feel on the simulator is impossible.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C

SYSCTL_RCGCGPIO_R  EQU 0x400FE608
       IMPORT  TExaS_Init
       AREA    |.text|, CODE, READONLY, ALIGN=2
       THUMB
       EXPORT  Start

Start
 ; TExaS_Init sets bus clock at 80 MHz
      BL  TExaS_Init ; voltmeter, scope on PD3
      CPSIE  I    ; TExaS voltmeter, scope runs on interrupts
	LDR R1, =SYSCTL_RCGCGPIO_R ;1) activate clock for Port F and E
	LDR R0, [R1]
	ORR R0, R0, #0x30 ;set bit 4,5 to turn on clock
	STR R0, [R1]
	NOP
	NOP ;allow time for clock to finish
	LDR R1, =GPIO_PORTF_DIR_R ;5) set direction register
	LDR R0, [R1]
	MOV R0, #0x00 ;set all pins as inputs
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_DEN_R ;7) enable Port F digital port
	LDR R0, [R1]
	MOV R0, #0x1B ;1 means enable digital I/O
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_AFSEL_R
	LDR R1, [R1]
	AND R0, R0, #0x00
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_PUR_R ;pull-up resistors for PF4
	MOV R0, #0x10 ;enable weak pull-up on PF4
	STR R0, [R1]

	LDR R1, =GPIO_PORTE_DIR_R ;5) set direction register
	LDR R0, [R1]
	MOV R0, #0x1 ;set pins 3-1 input, pin 0 as output
	STR R0, [R1]
	LDR R1, =GPIO_PORTE_DEN_R ;7) enable Port E digital port
	LDR R0, [R1]
	MOV R0, #0x1B ;1 means enable digital I/O
	STR R0, [R1]
	LDR R1, =GPIO_PORTE_AFSEL_R
	LDR R1, [R1]
	AND R0, R0, #0x00
	STR R0, [R1]

	MOV R11, #5	;PERIOD
	MOV R12, #1	;DUTY CYCLE

loop  
	LDR R1, =GPIO_PORTF_DATA_R
	LDR R2, [R1]
	AND R2, R2, #0x10
	AND R0, #0			;CLEAR R0
	CMP R2, R0
	BEQ breathe
	CMP R9, R0
	BGT noduty
	BL toggleLightOn		
noduty	
	ADD R0, R12, R0		;R0=DUTY
	BL delay
	BL delay
	SUB R0, R11, R12	;R0=PERIOD-DUTY
	AND R1, R1, #0
	CMP R0, R1
	BEQ fullcycle
	BL toggleLightOff
fullcycle
	BL delay
	BL delay
	LDR R1, =GPIO_PORTE_DATA_R	;CHECKING SWITCH 
	LDR R0, [R1]
	AND R2, R0, #0x02
	MOV R0, #0					;CREATING CONSTANT FOR COMPARING
	CMP R2, R0					;CHECKING SWITCH STATE
	BEQ notPressed
	AND R9, R9, #0
	AND R6, R6, #0				;CLEAR R6
	ADD R6, R6, #1			;INDICATES SWITCH HAS BEEN PRESSED
	B loop
notPressed
	AND R5, R5, #0				;CLEAR R5
	CMP R6, R5					;CHECK IF SWITCH HAS BEEN PRESSED
	BEQ loop
	CMP R11, R12				;CHECKS IF DUTY=PERIOD
	BEQ reset
	ADD R12, R12, #1			;IF NOT INCREASE DUTY (20%)
	AND R6, R6, #0
	B loop
reset
	AND R12, R12, #0			;RESETS DUTY TO 0%
	AND R6, R6, #0
	AND R9, R9, #0
	ADD R9, R9, #1
	B loop
	
breathe
	PUSH {R12, R7, R0, R11}
	MOV R11, #10
	MOV R7, #0			;SET COUNTER TO 0
	MOV R10, #0			;UP/DOWN INDICATOR (0 UP, 1 DOWN)
	MOV R1, #0			;0 CONSTANT FOR COMPARING
breathe1
	BL toggleLightOn		
	ADD R0, R12, R0		;R0=DUTY
	BL delay			
	BL toggleLightOff
	SUB R0, R11, R12	;R0=PERIOD-DUTY
	BL delay
	CMP R10, R1			;increment or decrement (up/down indicator)
	BEQ increment		;if R10==0
	ADD R12, R12, #-1	;decrement
	B skp
increment
	ADD R12, R12, #1
skp
	CMP R12, R11		;are we at top?
	BEQ top
	CMP R12, R1			;are we at bottom?
	BEQ bottom
	B fin
top
	MOV R10, #1			;set up/down indicator to down direction (1)
	B fin
bottom
	MOV R10, #0			;set up/down indicator to up direction (0)
	B fin
fin
	LDR R1, =GPIO_PORTF_DATA_R			;check if button is still pressed
	LDR R2, [R1]
	AND R2, R2, #0x10
	AND R0, #0			;CLEAR R0
	CMP R2, R0
	BEQ breathe1
	POP {R12, R7, R0, R11}
		B    loop


		
toggleLightOn
	PUSH {R0, R1, R2, LR}
	LDR R1, =GPIO_PORTE_DATA_R
	LDR R0, [R1]
	ORR R0, R0, #0x01				;SETTING PE0 TO 1
	STR R0, [R1]					;STORES NEW DATAREG
	POP {R0, R1, R2, LR}
	BX LR

toggleLightOff
	PUSH {R0, R1, R2, LR}
	LDR R1, =GPIO_PORTE_DATA_R
	LDR R0, [R1]
	AND R0, R0, #0xFE				;SETTING PE0 TO 0
	STR R0, [R1]					;STORES NEW DATAREG
	POP {R0, R1, R2, LR}
	BX LR

delay
	PUSH {LR, R1, R0, R2}
	;MOV R2, #40
	;MUL R0, R0, R2
while					
	MOV R1, #0						;CLEAR R1
	BL sub1							
	ADD R0, #-1						;DECREMTENTS WHILE LOOP INDICATOR
	CMP R0, R1
	BGT while						;REPEATS EACH 1/5 OF THE DUTY CYCLE
	POP {LR, R1, R0, R2}
	BX LR
	



sub1								;DELAYS FOR 1/80th OF PERIOD
	PUSH {R0,R1}
	MOV R0,#0xF424					;SETS R0 TO ARBIRTRARY CONSTANT WHICH WE FOUND WITH TRIAL AND ERROR
	MOV R1, R0						;SETS R1 TO R0
wait  
	SUBS R0,R0,#0x01				;DELAYS FOR A VERY SMALL AMOUNT OF TIME
	BNE  wait
wait1  
	SUBS R1,R1,#0x01
	BNE  wait1
	MOV R0,#0xF424
	MOV R1, R0
wait2  
		SUBS R0,R0,#0x01
	BNE  wait2
wait3  
	SUBS R1,R1,#0x01
	BNE  wait3	
	MOV R1, R0
	POP {R0, R1}
	BX LR
	
	ALIGN      ; make sure the end of this section is aligned
      END        ; end of file



