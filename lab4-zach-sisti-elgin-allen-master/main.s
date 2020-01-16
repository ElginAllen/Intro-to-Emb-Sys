;****************** main.s ***************
; Program written by: Elgin Allen
; Date Created: 2/14/2017
; Last Modified: 2/27/2017
; Brief description of the program
;   The LED toggles at 8 Hz and a varying duty-cycle
;   Repeat the functionality from Lab2-3 but now we want you to 
;   insert debugging instruments which gather data (state and timing)
;   to verify that the system is functioning as expected.
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE0 is LED output (1 activates external LED on protoboard)
;  PF2 is Blue LED on Launchpad used as a heartbeat
; Instrumentation data to be gathered is as follows:
; After Button(PE1) press collect one state and time entry. 
; After Buttin(PE1) release, collect 7 state and
; time entries on each change in state of the LED(PE0): 
; An entry is one 8-bit entry in the Data Buffer and one 
; 32-bit entry in the Time Buffer
;  The Data Buffer entry (byte) content has:
;    Lower nibble is state of LED (PE0)
;    Higher nibble is state of Button (PE1)
;  The Time Buffer entry (32-bit) has:
;    24-bit value of the SysTick's Current register (NVIC_ST_CURRENT_R)
; Note: The size of both buffers is 50 entries. Once you fill these
;       entries you should stop collecting data
; The heartbeat is an indicator of the running of the program. 
; On each iteration of the main loop of your program toggle the 
; LED to indicate that your code(system) is live (not stuck or dead).

GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C

GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_CR_R	   EQU 0x40025524
GPIO_PORTF_PCTL_R  EQU 0x4002552C
GPIO_PORTF_LOCK_R  EQU 0x40025520
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

NVIC_ST_CTRL_R        EQU 0xE000E010
NVIC_ST_RELOAD_R      EQU 0xE000E014
NVIC_ST_CURRENT_R     EQU 0xE000E018
; RAM Area
           AREA    DATA, ALIGN=2
;-UUU-Declare  and allocate space for your Buffers 
;    and any variables (like pointers and counters) here
DataBuffer SPACE 50		;each enty is 1 byte/8B
TimeBuffer SPACE 200	; each entry is 4 bytes/32B
DataPt SPACE 4			; Addresses are 32B
TimePt SPACE 4			;Addresses are 32B
NEntries FILL 1,0,1
; ROM Area
       IMPORT  TExaS_Init
	   IMPORT  SysTick_Init
;-UUU-Import routine(s) from other assembly files (like SysTick.s) here
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
	LDR R1, =GPIO_PORTF_LOCK_R
	LDR R0, =0x4C4F434B
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_LOCK_R
	LDR R0, [R1]
	LDR R1, =GPIO_PORTF_PCTL_R	
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_CR_R
	MOV R0, #0x7FFF
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_DIR_R ;5) set direction register
	LDR R0, [R1]
	MOV R0, #0x04 ;set all pins as inputs
	STR R0, [R1]
	LDR R1, =GPIO_PORTF_DEN_R ;7) enable Port F digital port
	LDR R0, [R1]
	MOV R0, #0x1F ;1 means enable digital I/O
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
	MOV R10, #0
	MOV R11, #5	;PERIOD
	MOV R12, #1	;DUTY CYCLE
	BL Debug_Init
loop  
	LDR R1, =GPIO_PORTF_DATA_R
	LDR R2, [R1]
	AND R2, R2, #0x04
	EOR R2, R2, #0X04
	STR R2, [R1]
	
	
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
	ADD R8, R8, #1
	;CMP R8, #0
	;BEQ DontCap
	;BL Debug_Capture
	;SUB R8, R8, #1
;DontCap	
	B loop
notPressed
	AND R8, R8, #0
	AND R5, R5, #0				;CLEAR R5
	CMP R6, R5					;CHECK IF SWITCH HAS BEEN PRESSED
	BEQ loop
	MOV R10, #7
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
	B loop

		
toggleLightOn
	PUSH {R0, R1, R2, LR}	
	LDR R1, =GPIO_PORTE_DATA_R
	LDR R0, [R1]
	ORR R0, R0, #0x01				;SETTING PE0 TO 1
	STR R0, [R1]					;STORES NEW DATAREG
	CMP R8, #1
	BNE NoCNo
	BL Debug_Capture
	MOV R8, #-250
NoCNo
	CMP R10, #0
	BEQ nocap
	BL Debug_Capture
	SUB R10, R10, #1
nocap	
	POP {R0, R1, R2, LR}
	BX LR

toggleLightOff
	PUSH {R0, R1, R2, LR}
	
	LDR R1, =GPIO_PORTE_DATA_R
	LDR R0, [R1]
	AND R0, R0, #0xFE				;SETTING PE0 TO 0
	STR R0, [R1]					;STORES NEW DATAREG
	CMP R8, #0
	BNE nocap1
	CMP R10, #0
	BEQ nocap1
	BL Debug_Capture
	SUB R10, R10, #1
nocap1	
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
	
	
Debug_Init
	PUSH {R0, R1, R2, R3, R8, R9, LR, R10}
	LDR R0, =DataBuffer		
	LDR R1, =TimeBuffer
	MOV R3, #50				;Loop counter
ClearLoop
	CMP R3, #0				;Check if at end of array
	BEQ ExitClear
	
	MOV R8, #0xFF		;set current Data array value to FF
	STR R8, [R0]
	
	MOV R9, #0xFFFFFFFF	;set current Time array value to 8 Fs
	STR R9, [R1]
	
	ADD R0, R0, #1		;Move to next value in Data array
	ADD R1, R1, #4		;Move to next value in TIme Array
	SUB R3, R3, #1
	BL ClearLoop
ExitClear

	LDR R0, =DataBuffer
	LDR R1, =DataPt
	STR R0, [R1]		;Set pointer at start of array
	
	LDR R0, =TimeBuffer
	LDR R1, =TimePt
	STR R0, [R1]		;Set pointer at start of array
	
	BL SysTick_Init
	POP {R0, R1, R2, R3, R8, R9, LR, R10}
	BX LR

Debug_Capture
	PUSH {R0, R1, R2, R3, R4, R5}
	LDR R0, =NEntries
	LDR R1, [R0]
	CMP R1, #50
	BEQ FIN
	
	LDR R0, =GPIO_PORTE_DATA_R	
	LDR R1, [R0]
	AND R1, R1, #0x03
	
	LDR R0, =NVIC_ST_CURRENT_R
	LDR R4, [R0]
	
	LSL R2, R1, #3
	AND R2, R2, #0x10
	AND R1, R1, #0x01	;prep data to be dumped
	ADD R1, R1, R2
	
	LDR R0, =DataPt
	LDR R3, [R0]		;store data
	STRB R1, [R3]
	
	ADD R3, R3, #1		;increment DataPt
	STR R3, [R0] 
	
	LDR R0, =TimePt
	LDR R3, [R0]		;store Time
	STR R4, [R3]
	
	ADD R3, R3, #4		;increment TimePt
	STR R3, [R0]
	
	LDR R0, =NEntries
	LDR R1, [R0]
	ADD R1, R1, #1
	STR R1, [R0]

FIN	
	POP {R0, R1, R2, R3, R4, R5}
	BX LR
	
      ALIGN      ; make sure the end of this section is aligned
      END        ; end of file

