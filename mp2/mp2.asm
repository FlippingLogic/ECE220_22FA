; Author: Yu,Siying
; Identity: ECE220 MP2
; Last Modified: 2022/10/6
; Function: 1.Translates a student’s daily schedule from a list to a two-
;             dimensional matrix of pointers (memory addresses, in this
;             case to names of events).
;           2.Prints the schedule as shown below.
;                 | Mon  | Tue  | Wed  | Thu  | Fri  
;           07:00 |      |      |      |      |      
;           08:00 |      |      |      |      |      
;           09:00 | M286 | M286 | M286 | M286 |CLCV d
;           10:00 | 210  | 210  | 210  |      | 210  
;           11:00 |lunch |      |      |      |      
;           12:00 | 210  |lunch |lunch |lunch |lunch 
;           13:00 | lab  |CLCV 1|      |CLCV 1|      
;           14:00 |      |ECE220|      |ECE220|      
;           15:00 |study |      |      |      |      
;           16:00 | with |      |      |      |220 la
;           17:00 |friend|      |      |      |      
;           18:00 |dinner|dinner|dinner|dinner|dinner
;           19:00 |      |      |      |      |      
;           20:00 |      |      |      |      | date 
;           21:00 |      |      |      |      |night 
;           22:00 |      |      |      |      |      

    .ORIG x3000

; PART1: INITIALIZATION - Clear the memory for translation list
; R0: Carry x0000, the content to initialize
; R1: Pointer to the translation location
; R2: Counter for the 80 loops
    AND R0,R0,#0        ; Initialize the value container
    LD  R1,TRANS        ; Initialize the pointer
    LD  R2,NUM          ; Initialize the counter
INIT
    STR R0,R1,#0        ; Initialize the memory
    ADD R1,R1,#1        ; Increment the pointer
    ADD R2,R2,#-1       ; Decrement the counter
    BRp INIT            ; Enumerate all 80 memories

; PART2: TRANSLATION - Translate data into two dimensional matrix
; R0: Pointer for given list
; R1: Pointer for current event
; R2: Counter for the 5 days loop
; R3: Store content loaded from the memory
; R4: Mask
; R5: Store time for current event
; R6: Temporary use for branch judgement
; R7: Temporary use for corresponding translated address
    LD  R0,LIST         ; Initialize the data pointer
EVENT
    AND R2,R2,#0        ; Initialize the counter
    LD  R4,MASK         ; Initialize the mask
    ADD R1,R0,#0        ; Save starting label address
LABEL                   ; Loop until the label ends
    ADD R0,R0,#1        ; Increment the pointer
    LDR R3,R0,#0
    BRnp LABEL          
    ADD R0,R0,#2        ; Points to the event's time
    LDR R5,R0,#0        ; Store the time
    BRn WARN_INVALID    ; Error Detect, if R5<0 or R5>15 -> warn and halt
    ADD R6,R5,#-15
    BRp WARN_INVALID
    ADD R6,R5,R5        
    ADD R6,R6,R6        
    ADD R5,R5,R6        ; R5 <- 5*R5
    ADD R0,R0,#-1       ; Points to the event's day
    LDR R3,R0,#0        ; Store the day
DAY
    AND R6,R3,R4        ; Mask R3 by R4
    BRz DAY_END         ; No event -> move to next bit
    LD  R7,TRANS
    ADD R7,R7,R5        ; Add time bias
    ADD R7,R7,R2        ; Add day bias
    LDR R6,R7,#0
    BRnp WARN_OCCUPY    ; Error Detect, if M[R7]!=0 -> warn and halt
    STR R1,R7,#0        ; Write label address into translated list
DAY_END
    ADD R4,R4,R4        ; Left move the mask
    ADD R2,R2,#1        ; Increment the counter
    ADD R6,R2,#-5
    BRn DAY             ; Enumerate all 5 bits
    ADD R0,R0,#2        ; Point to next event
    LDR R6,R0,#0
    BRnp EVENT          ; empty string -> translation part ends

; PART3: PRINTING - Print the schedule to the screen
; R0: Used for OUT and PUTS
; R1: Pass parameter to subroutines
; R2: Counter for the 5 days loop
; R3: Pointer to the translation list
; R5: Counter for the 15 hours loop
; R6: Temporary use for branch judgement
; R7: Reserved for TRAP
    AND R2,R2,#0        ; Initialize the day counter
    LEA R1,BLANK
    JSR PRINT_CENTERED  ; Print empty string
    LEA R1,HEADER       ; Initialize the header pointer
INFO
    LD  R0,BAR
    OUT                 ; Print "|"
    JSR PRINT_CENTERED  ; Print current header
    ADD R1,R1,#4        ; Increment header pointer
    ADD R2,R2,#1        ; Increment day counter
    ADD R6,R2,#-5
    BRn INFO            ; Enumerate all 5 headers`
    LD  R0,LINEFEED
    OUT                 ; Print linefeed
    LD  R3,TRANS        ; Initialize translated list pointer
    LD  R5,MINUS        ; Initialize time counter
SCHEDULE
    ADD R5,R5,#1        ; Increment time counter
    ADD R1,R5,#0
    JSR PRINT_SLOT
    LD  R2,MINUS        ; Initialize day counter
WEEK_LOOP
    LD  R0,BAR
    OUT                 ; Print "|"
    ADD R2,R2,#1        ; Increment day counter
    LDR R1,R3,#0
    BRnp PRINT          ; Exist event -> load label
    LEA R1,BLANK        ; No event -> load empty string
PRINT
    JSR PRINT_CENTERED  ; Print label or blank string
    ADD R3,R3,#1        ; Increment the translated list pointer
    ADD R6,R2,#-4
    BRn WEEK_LOOP       ; Enumerate all 5 days
    LD  R0,LINEFEED
    OUT                 ; Print linefeed
    ADD R6,R5,#-15
    BRn SCHEDULE        ; Enumerate all 15 hours
    HALT

WARN_INVALID
    ADD R0,R1,#0
    PUTS                ; Print label of invalid event
    LEA R0,INVALID
    PUTS                ; Print warning message
    HALT
WARN_OCCUPY
    ADD R0,R1,#0
    PUTS                ; Print label of invalid event
    LEA R0,OCCUPY
    PUTS                ; Print warning message
    HALT

BLANK   .STRINGZ ""
BAR     .FILL x007C
LINEFEED.FILL x000A
TRANS   .FILL x4000
LIST    .FILL x5000
MASK    .FILL x0001
NUM     .FILL #80
OCCUPY  .STRINGZ " conflicts with an earlier event.\n"
INVALID .STRINGZ " has an invalid slot number.\n"
HEADER  .STRINGZ "Mon"
        .STRINGZ "Tue"
        .STRINGZ "Wed"
        .STRINGZ "Thu"
        .STRINGZ "Fri"


; PRINT_SLOT subroutine: Print specific time slot to screen
; Input: R1 (A number from 0-15)
; Output: Print "R1+7:00" to screen
; Side Effect: None
; Register Usage: R0: Store value loaded from the memory & Store the value to be printed
;                 R1: Parameter passed from main routine, indicating the time-7
;                 R6: Temporary used for branch judgement
;                 R7: Reserved for TRAP instruction
PRINT_SLOT
    ; Store the value of registers into memory
    ST R0,SLO_R0
    ST R1,SLO_R1
    ST R6,SLO_R6
    ST R7,SLO_R7
    ; Main subroutine    
    LD  R0,ZERO         ; Load the ACSII value for zero
    ADD R1,R1,#-3
    BRn GENERAL         ; R1<3 -> first character "0"
    ADD R0,R0,#1        ; 3<R1<13 -> first character "1"
    ADD R1,R1,#-10      ; Decrement the bias
    BRn GENERAL
    ADD R0,R0,#1        ; R1>13 -> first character "2"
    ADD R1,R1,#-10      ; Decrement the bias
GENERAL
    OUT                 ; Print first number
    LD  R6,BIAS
    ADD R0,R1,R6        ; Compute the 2nd number's ASCII value
    OUT                 ; Print 2nd number
    LD R0,BIAS
    OUT                 ; Print ":"
    LD R0,ZERO
    OUT                 ; Print "0"
    OUT                 ; Print "0"
    LD R0,SPACE
    OUT                 ; Print space
    LD R0,SLO_R0        ; Load the value of registers from memory
    LD R1,SLO_R1
    LD R6,SLO_R6
    LD R7,SLO_R7
    RET                 ; Branch back to main routine
    

; PRINT_CENTERED subroutine: Print the centerd string(length<=6) to screen
; Input: R1 (The starting address of the string)
; Output: Print the centerd string(length<=6) to screen
; Side Effect: None
; Register Usage: R0: Store value loaded from the memory & Store the value to be printed
;                 R1: Store starting address of the string
;                 R2: Counting counter
;                 R3: Printing counter
;                 R4: Character pointer
;                 R6: Temporary Use for branch judgement
;                 R7: Reserved for TRAP instruction
PRINT_CENTERED
    ; Store the value of registers into memory
    ST R0,CEN_R0
    ST R1,CEN_R1
    ST R2,CEN_R2
    ST R3,CEN_R3
    ST R4,CEN_R4
    ST R6,CEN_R6
    ST R7,CEN_R7
    ; Main subroutine  
    LD  R2,MINUS        ; Initialize counter
COUNT
    ADD R2,R2,#1        ; Increment counter
    ADD R4,R1,R2        ; Compute address
    LDR R0,R4,#0
    BRnp COUNT          ; M[R4]!=0x00 -> string doesn't end
    ADD R6,R2,#-6
    BRn SHORT           ; Branch to code for string shorter than 6

    ; Code for string longer than 6
    LD  R3,MINUS        ; Initialize counter
MAIN_LONG
    ADD R3,R3,#1        ; Increment the counter
    ADD R4,R1,R3        ; Compute the address
    LDR R0,R4,#0
    OUT                 ; Print the character
    ADD R6,R3,#-5
    BRn MAIN_LONG       ; Enumerate all 6 characters
    BRnzp DONE

    ; Code for string shorter than 6
SHORT
    LD  R0,SPACE
    ADD R6,R2,#-5
    BRzp MAIN_SHORT     ; len(string)>5 -> skip front spaces
FRONT
    OUT                 ; Print a space
    ADD R6,R6,#2        ; Print one extra space for every 2 shorter length
    BRn    FRONT
MAIN_SHORT
    ADD R0,R1,#0
    PUTS                ; Print the string
    LD  R0,SPACE
    ADD R2,R2,#-6       ; Compute the difference between length and 6
TRAIL
    OUT                 ; Print a space
    ADD R2,R2,#2        ; Print one extra space for every 2 shorter length
    BRn TRAIL
DONE
    ; Load the value of registers from memory
    LD R0,CEN_R0
    LD R1,CEN_R1
    LD R2,CEN_R2
    LD R3,CEN_R3
    LD R4,CEN_R4
    LD R6,CEN_R6
    LD R7,CEN_R7
    RET                 ; Branch back to main routine

ZERO        .FILL   x0030
BIAS        .FILL   x003A
SPACE       .FILL   x0020
MINUS       .FILL   #-1
SLO_R0      .BLKW   #1
SLO_R1      .BLKW   #1
SLO_R6      .BLKW   #1
SLO_R7      .BLKW   #1
CEN_R0      .BLKW   #1
CEN_R1      .BLKW   #1
CEN_R2      .BLKW   #1
CEN_R3      .BLKW   #1
CEN_R4      .BLKW   #1
CEN_R6      .BLKW   #1
CEN_R7      .BLKW   #1

    .END