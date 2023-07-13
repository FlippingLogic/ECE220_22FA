; Created by Yu,Siying, 2022/9/19
; Identity: ECE220 MP1

; R0: Holds the ASCII value loaded from the memory & Character to be printed
;     Pointer when saving register value before the subroutines
; R1: Holds value from 0 to 15, indicating the time-7 (Subrotine PRINT_SLOT)
;     Holds the string starting address (Subrotine PRINT_CENTERED)
; R2: Character number counter for counting part
; R3: Character number counter for printing part
; R4: Character pointer
; R5: Unused
; R6: Temporary Use
; R7: Reserved for JSR instruction
    
    .ORIG	x3000		; starting address is x3000

; Test Code
; Test Code
; Test Code
	; feeling lazy, so I'm going to set all of the bits to the same value
	LD	R0,BITS
	ADD	R2,R0,#0
	ADD	R3,R0,#0
	ADD	R4,R0,#0
	ADD	R5,R0,#0
	ADD	R6,R0,#0
	; let's try PRINT_SLOT ... 11:00
	AND	R1,R1,#0
	ADD	R1,R1,#4
	; set a breakpoint here in the debugger, then use 'next' to
	; execute your subroutine and see what happens to the registers;
	; they're not supposed to change (except for R7)...
	JSR	PRINT_SLOT
	; we're short on human time to test your code, so we'll do 
	; something like the following instead (feel free to replicate)...
	LD	R7,BITS
	NOT	R7,R7
	ADD	R7,R7,#1
	ADD	R0,R0,R7
	BRz	R0_OK
	LEA	R0,R0_BAD
	PUTS
R0_OK	
	; this trap changes register values, so it's not sufficient
	; to check that all of the registers are unchanged; HALT may
	; also lead to confusion because the register values differ
	; for other reasons (R7 differences, for example).
	HALT
BITS	.FILL	xABCD	; something unusual
VLINE	.FILL	x7C	; ASCII vertical line character
R0_BAD	.STRINGZ "PRINT_SLOT changes R0!\n"
; Test Code
; Test Code
; Test Code


; PRINT_SLOT subroutine: Print specific time slot to screen
; Input: R1 (A number from 0-15)
; Output: Print "R1+7:00" to screen
; Side Effect: None
PRINT_SLOT
    ST  R0,REGISTER ; Save original value of R0 to memory
    LEA R0,REGISTER ; R0 is the register saving pointer
    STR R1,R0,#1    ; Save original value of R1 to memory
    STR R2,R0,#2    ; Save original value of R2 to memory
    STR R3,R0,#3    ; Save original value of R3 to memory
    STR R4,R0,#4    ; Save original value of R4 to memory
    STR R5,R0,#5    ; Save original value of R5 to memory
    STR R6,R0,#6    ; Save original value of R6 to memory
    STR R7,R0,#7    ; Save original value of R7 to memory
    LD  R0,ZERO     ; Load the ACSII value for zero
    ADD R1,R1,#-3   ; Determine whether R1 is larger than 3 for not
    BRn GENERAL     ; If R1(original)<3, then the first number is 0
    ADD R0,R0,#1    ; If 3<R1(original)<13, then the first number is 1
    ADD R1,R1,#-10  ; Decrement the second number's bias by 10
    BRn GENERAL     ; Determine whether R1(original) is larger than 13 for not
    ADD R0,R0,#1    ; R1(original)>13, the first number is 2
    ADD R1,R1,#-10  ; Decrement the second number's bias by 10
GENERAL
    OUT         ; Print the first number
    LD  R6,BIAS ; Load the differce between ASCII for target number and current number in R1
    ADD R0,R1,R6; Compute the ASCII value for target number
    OUT         ; Print the second number
    LD R0,BIAS  ; The ACSII for column(:) equals to the bias (x3A)
    OUT         ; Print the column
    LD R0,ZERO  ; Load the ASCII value for zero
    OUT         ; Print the first zero
    OUT         ; Print the second zero
    LD R0,SPACE ; Load the ASCII value for space
    OUT         ; Print a space
    BRnzp   DONE
    

; PRINT_CENTERED subroutine: Print the centerd string(length<=6) to screen
; Input: R1 (The starting address of the string)
; Output: Print the centerd string(length<=6) to screen
; Side Effect: None
PRINT_CENTERED
    ST  R0,REGISTER ; Save original value of R0 to memory
    LEA R0,REGISTER ; R0 is the register saving pointer
    STR R1,R0,#1    ; Save original value of R1 to memory
    STR R2,R0,#2    ; Save original value of R2 to memory
    STR R3,R0,#3    ; Save original value of R3 to memory
    STR R4,R0,#4    ; Save original value of R4 to memory
    STR R5,R0,#5    ; Save original value of R5 to memory
    STR R6,R0,#6    ; Save original value of R6 to memory
    STR R7,R0,#7    ; Save original value of R7 to memory
    LD  R2,MINUS    ; Initialize counter to -1
COUNT
    ADD R2,R2,#1    ; Increment the counter
    ADD R4,R1,R2    ; Compute the address by add R1(Base address) and R2(bias)
    LDR R0,R4,#0    ; Load ASCII value to R0
    BRnp COUNT      ; If M[R4]!=0x00, the string doesn't end

    ADD R6,R2,#-6   ; Determine if the string is longer than 6 or not
    BRn SHORT       ; Branch to code for string shorter than 6

; Code for string longer than 6
    LD  R3,MINUS    ; Initialize counter to -1
MAIN_LONG
    ADD R3,R3,#1    ; Increment the counter
    ADD R4,R1,R3    ; Compute the address by add R1(Base address) and R2(bias)
    LDR R0,R4,#0    ; Load ASCII value to R0
    OUT             ; Print the character to the screen
    ADD R6,R3,#-5   ; Determine if 6 character has already be printed
    BRn MAIN_LONG   ; Branch to print the next character
    BRnzp   DONE    ; All work done

; Code for string shorter than 6
SHORT
    LD  R0,SPACE    ; Load ASCII value for space to R0
    ADD R6,R2,#-5   ; Determine if the string length is longer than 5 or not
    BRzp MAIN_SHORT ; Branch to print the string
FRONT
    OUT             ; Print a space
    ADD R6,R6,#2    ; Determine if one more space is needed
    BRn    FRONT   ; Branch to print the next space
MAIN_SHORT
    ADD R0,R1,#0    ; Load the string starting address to R0
    PUTS            ; Print the string
    LD  R0,SPACE    ; Load ASCII value for space to R0
    ADD R2,R2,#-6   ; Decide the difference between current length and 6
TRAIL
    OUT             ; Print a space
    ADD R2,R2,#2    ; etermine if one more space is needed
    BRn TRAIL       ; Branch to print the next space

DONE
    LEA R0,REGISTER ; R0 is the register saving pointer
    LDR R1,R0,#1    ; Load the original value of R1 from memory
    LDR R2,R0,#2    ; Load the original value of R2 from memory
    LDR R3,R0,#3    ; Load the original value of R3 from memory
    LDR R4,R0,#4    ; Load the original value of R4 from memory
    LDR R5,R0,#5    ; Load the original value of R5 from memory
    LDR R6,R0,#6    ; Load the original value of R6 from memory
    LDR R7,R0,#7    ; Load the original value of R7 from memory
    LD  R0,REGISTER ; Load the original value of R0 from memory
    RET             ; Branch back to the main routine

ZERO        .FILL   x0030   ; ASCII value for zero
BIAS        .FILL   x003A   ; ASCII value for column
SPACE       .FILL   x0020   ; ASCII value for space
MINUS       .FILL   #-1     ; Used for register initialization
REGISTER    .BLKW   #8      ; space to store the origianl register value

    .END