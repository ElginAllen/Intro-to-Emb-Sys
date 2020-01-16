; Print.s
; Student names: change this to your names or look very silly
; Last modification date: change this to the last modification date or look very silly
; Runs on LM4F120 or TM4C123
; EE319K lab 7 device driver for any LCD
;
; As part of Lab 7, students need to implement these LCD_OutDec and LCD_OutFix
; This driver assumes two low-level LCD functions
; ST7735_OutChar   outputs a single 8-bit ASCII character
; ST7735_OutString outputs a null-terminated string 

   

	IMPORT   ST7735_OutChar
    IMPORT   ST7735_OutString
    EXPORT   LCD_OutDec
    EXPORT   LCD_OutFix

    AREA    |.text|, CODE, READONLY, ALIGN=2
    THUMB

  
ASCII				EQU 	0
ASCII1				EQU		4
ASCII2				EQU		8
ASCII3				EQU		12
ASCII4				EQU		16
ASCII5				EQU		20

;-----------------------LCD_OutDec-----------------------
; Output a 32-bit number in unsigned decimal format
; Input: R0 (call by value) 32-bit unsigned number
; Output: none
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutDec
	PUSH {R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,LR,R0}
	SUB SP, SP, #24
	MOV R1, #10000
	MUL R1, R1, R1
	MOV R2, #10
	MUL R1, R1, R2
	CMP R0, R1
	BGE BIL
	CMP R0, #0
	BLT BIL
	MOV R1, #10000
	MUL R1, R1, R1
	CMP R0, R1
	BGE HUNMIL
	MOV R1, #10000
	MOV R2, #1000
	MUL R1, R1, R2
	CMP R0, R1
	BGE TENMIL
	MOV R1, #10000
	MOV R2, #100
	MUL R1, R1, R2
	CMP R0, R1
	BGE MIL
	MOV R1, #10000
	MOV R2, #10
	MUL R1, R1, R2
	CMP R0, R1
	BGE HUNTHOU
	MOV R1, #10000
	CMP R0, R1
	BGE TENTHOU
	CMP R0, #1000
	BGE THOU
	CMP R0, #100
	BGE HUN
	CMP R0, #10
	BGE TEN
	MOV R2, #1
	MOV R1, R0
	B ONES
BIL
	MOV R2, #10
	B LOOP
HUNMIL
	MOV R2, #9
	B LOOP
TENMIL
	MOV R2, #8
	B LOOP
MIL
	MOV R2, #7
	B LOOP
HUNTHOU
	MOV R2, #6
	B LOOP
TENTHOU
	MOV R2, #5
	B LOOP
THOU
	MOV R2, #4
	B LOOP
HUN
	MOV R2, #3
	B LOOP
TEN
	MOV R2, #2
LOOP
	MOV R1, R0
	CMP R2, #1
	BEQ ONES
	MOV R3, #1
	MOV R4, R2
WHILE
	MOV R6, #10
	MUL R3, R3, R6
	SUB R4, R4, #1
	CMP R4, #1
	BGT WHILE
	UDIV R1, R1, R3
ONES
	ADD R1, #0x30
	STR R1, [SP, #ASCII]
	BL OUTPUT
	SUB R1, #0x30
	MUL R1, R1, R3
	SUB R0, R0, R1
	SUB R2, R2, #1
	CMP R2, #1
	BLT DONE
	B LOOP
DONE
	ADD SP, SP, #24
	POP {R1,R2,R3,R4,R5,R6,R7,R8,R9,R10,R11,R12,LR,R0}
    BX  LR
OUTPUT
	MOV R7, R0
	MOV R8, LR
	MOV R9, R1
	MOV R10, R2
	MOV R11, R3
	MOV R12, R4
	LDR R0, [SP, #ASCII]
	BL ST7735_OutChar
	MOV R0, R7
	MOV LR, R8
	MOV R1, R9
	MOV R2, R10
	MOV R3, R11
	MOV R4, R12
	BX LR
;* * * * * * * * End of LCD_OutDec * * * * * * * *

; -----------------------LCD _OutFix----------------------
; Output characters to LCD display in fixed-point format
; unsigned decimal, resolution 0.001, range 0.000 to 9.999
; Inputs:  R0 is an unsigned 32-bit number
; Outputs: none
; E.g., R0=0,    then output "0.000 "
;       R0=3,    then output "0.003 "
;       R0=89,   then output "0.089 "
;       R0=123,  then output "0.123 "
;       R0=9999, then output "9.999 "
;       R0>9999, then output "*.*** "
; Invariables: This function must not permanently modify registers R4 to R11
LCD_OutFix
	PUSH {R1,R2,R3,R4,R5,R6,R7,R8,R9,LR}
	SUB SP, SP, #24
	MOV R5 ,#0
	MOV R1, #0x30
	STR R1, [SP, #ASCII1]
	STR R1, [SP, #ASCII3]
	STR R1, [SP, #ASCII4]
	STR R1, [SP, #ASCII5]
	MOV R1, #0x2E
	STR R1, [SP, #ASCII2]
	MOV R1, #10000
	CMP R0, R1
	BGE TENTHOU1
	CMP R0, #0
	BLT TENTHOU1
	CMP R0, #1000
	BGE THOU1
	CMP R0, #100
	BGE HUN1
	CMP R0, #10
	BGE TEN1
	MOV R2, #0
	MOV R1, R0
	B ONES1
TENTHOU1
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2E
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	MOV R0, #0x2A
	BL ST7735_OutChar
	B FINALOVERTENTHOUSAND
THOU1
	MOV R2, #3
	B LOOP1
HUN1
	MOV R2, #2
	B LOOP1
TEN1
	MOV R2, #1
LOOP1
	MOV R1, R0
	CMP R2, #0
	BEQ ONES1
	MOV R3, #1
	MOV R4, R2
WHILE1
	MOV R6, #10
	MUL R3, R3, R6
	SUB R4, R4, #1
	CMP R4, #0
	BGT WHILE1
	UDIV R1, R1, R3
ONES1
	ADD R1, #0x30
	ADD R5, R5, #1
	BL OUTPUT1
	SUB R1, #0x30
	MUL R1, R1, R3
	SUB R0, R0, R1
	SUB R2, R2, #1
	CMP R2, #0
	BLT DONE1
	B LOOP1
DONE1
	LDR R0, [SP, #ASCII1]
	BL ST7735_OutChar
	LDR R0, [SP, #ASCII2]
	BL ST7735_OutChar
	LDR R0, [SP, #ASCII3]
	BL ST7735_OutChar
	LDR R0, [SP, #ASCII4]
	BL ST7735_OutChar
	LDR R0, [SP, #ASCII5]
	BL ST7735_OutChar	
FINALOVERTENTHOUSAND
	ADD SP, SP, #24
	POP {R1,R2,R3,R4,R5,R6,R7,R8,R9,LR}
    BX  LR
OUTPUT1
	MOV R7, R1
	MOV R8, R2
	MOV R9, LR
	CMP R5, #2
	BGE SKP
	STR R1, [SP, #ASCII5]
	B FIN
SKP	
	CMP R5, #3
	BGE SKP1
	LDR R2, [SP, #ASCII5]
	STR R2, [SP, #ASCII4]
	STR R1, [SP, #ASCII5]
	B FIN
SKP1	
	CMP R5, #4
	BGE SKP2
	LDR R2, [SP, #ASCII4]
	STR R2, [SP, #ASCII3]
	LDR R2, [SP, #ASCII5]
	STR R2, [SP, #ASCII4]
	STR R1, [SP, #ASCII5]
	B FIN
SKP2	
	LDR R2, [SP, #ASCII3]
	STR R2, [SP, #ASCII1]
	LDR R2, [SP, #ASCII4]
	STR R2, [SP, #ASCII3]
	LDR R2, [SP, #ASCII5]
	STR R2, [SP, #ASCII4]
	STR R1, [SP, #ASCII5]
FIN
	MOV R1, R7
	MOV R2, R8
	MOV LR, R9
	BX LR
 
     ALIGN
;* * * * * * * * End of LCD_OutFix * * * * * * * *

     ALIGN                           ; make sure the end of this section is aligned
     END                             ; end of file
