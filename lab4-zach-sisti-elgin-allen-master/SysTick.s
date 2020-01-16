; SysTick.s
; Module written by: Zach Sisti and Elgin Allen
; Date Created: 2/14/2017
; Last Modified: 2/27/2017 
; Brief Description: Initializes SysTick

NVIC_ST_CTRL_R        EQU 0xE000E010
NVIC_ST_RELOAD_R      EQU 0xE000E014
NVIC_ST_CURRENT_R     EQU 0xE000E018

        AREA    |.text|, CODE, READONLY, ALIGN=2
        THUMB
; ;-UUU-Export routine(s) from SysTick.s to callers
		EXPORT SysTick_Init 
;------------SysTick_Init------------
; ;-UUU-Complete this subroutine
; Initialize SysTick with busy wait running at bus clock.
; Input: none
; Output: none
; Modifies: ??
SysTick_Init
	LDR R0, =NVIC_ST_CTRL_R
	MOV R1, #0
	STR R1, [R0]
	;SPECIFY LOAD VALUE
	LDR R0, =NVIC_ST_RELOAD_R 
	LDR R1, =0x00FFFFFF
	STR R1, [R0]
	
	LDR R0, =NVIC_ST_CURRENT_R
	MOV R1, #0
	STR R1, [R0]
	
	LDR R0, =NVIC_ST_CTRL_R
	MOV R1, #0x05
	STR R1, [R0]
		
    BX  LR                          ; return


    ALIGN                           ; make sure the end of this section is aligned
    END                             ; end of file
