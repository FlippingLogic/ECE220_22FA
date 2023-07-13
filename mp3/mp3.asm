; Author: Yu,Siying
; Identity: ECE220 MP3
; Last Modified: 2022/10/16
; Function: For fixed schedule at x5000, extra list at x6000, 
;           attempting to find a compatible combination of times 
;           at which extra events can be inserted into an existing 
;           weekly schedule. Translate the merged valid schedule  
;           at x4000 and print it in standard format to screen.

    .ORIG x3000

    ; Main Program
    JSR INITIALIZE
    JSR TRANSLATION
    ADD R6,R0,#0        ; Subroutine status detection 
    BRp MAIN_INVALID
    BRn MAIN_OCCUPY
    JSR INSERTING
    ADD R6,R0,#0        ; Subroutine status detection 
    BRp MAIN_NONE
    JSR PRINTING
    HALT
    ; Raise Error
MAIN_INVALID
    LD  R0,WRONG_EVENT
    PUTS                ; Print label of invalid event
    LEA R0,INVALID
    PUTS                ; Print warning message
    HALT
MAIN_OCCUPY
    LD  R0,WRONG_EVENT
    PUTS                ; Print label of invalid event
    LEA R0,OCCUPY
    PUTS                ; Print warning message
    HALT
MAIN_NONE
    LEA R0,NONE
    PUTS                ; Print warning message
    HALT

INVALID     .STRINGZ " has an invalid slot number.\n"
OCCUPY      .STRINGZ " conflicts with an earlier event.\n"
NONE        .STRINGZ "Could not fit all events into schedule.\n"


; INITIALIZE subroutine
; Function: Initialize the schedule in memory locations x4000 through x404F 
;           to contain all NULL pointers (x0000) initially
; Side Effect: None
; Register Usage: R0: Carry x0000, the content to initialize
;                 R1: Pointer to the translation location
;                 R2: Counter for the 80 loops
INITIALIZE
    ST  R0,INI_R0       ; Save register value
    ST  R1,INI_R1
    ST  R2,INI_R2
    ST  R7,INI_R7
    AND R0,R0,#0        ; Initialize the value container
    LD  R1,PLACE        ; Initialize the pointer
    LD  R2,NUM          ; Initialize the counter
INIT
    STR R0,R1,#0        ; Clear the memory
    ADD R1,R1,#1        ; Increment the pointer
    ADD R2,R2,#-1       ; Decrement the counter
    BRp INIT            ; Enumerate all 80 memories
    LD  R0,INI_R0       ; Load register value
    LD  R1,INI_R1
    LD  R2,INI_R2
    LD  R7,INI_R7
    RET

INI_R0  .BLKW #1
INI_R1  .BLKW #1
INI_R2  .BLKW #1
INI_R7  .BLKW #1
NUM     .FILL #80
PLACE   .FILL x4000


; TRANSLATION subroutine
; Function: Translate event list at x5000 into two dimensional matrix at x4000
; Input: None
; Output: R0 - Indicate failure or success of the subroutines
;             (0: Success; 1: Invalid timeslot; -1: Event conflict)
; Side Effect: R0 would be modified
; Register Usage: R0: Pointer for given list
;                 R1: Pointer for current event
;                 R2: Counter for the 5 days loop
;                 R3: Store content loaded from the memory
;                 R4: Mask
;                 R5: Store time for current event
;                 R6: Temporary use for branch judgement
;                 R7: Temporary use for corresponding translated address
TRANSLATION
    ST R1,TRA_R1        ; Save register value
    ST R2,TRA_R2
    ST R3,TRA_R3
    ST R4,TRA_R4
    ST R5,TRA_R5
    ST R6,TRA_R6
    ST R7,TRA_R7
    LD  R0,DATA         ; Initialize the data pointer
EVENT
    AND R2,R2,#0        ; Initialize the counter
    LD  R4,TRA_MASK     ; Initialize the mask
    ADD R1,R0,#0        ; Save starting label address
LABEL                   ; Loop until the label ends
    ADD R0,R0,#1        ; Increment the pointer
    LDR R3,R0,#0
    BRnp LABEL          
    ADD R0,R0,#2        ; Points to the event's time
    LDR R5,R0,#0        ; Store the time
    BRn SUB_INVALID     ; Error Detect, if R5<0 or R5>15 -> warn and halt
    ADD R6,R5,#-15
    BRp SUB_INVALID
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
    BRnp SUB_OCCUPY     ; Error Detect, if M[R7]!=0 -> warn and halt
    STR R1,R7,#0        ; Write label address into translated list
DAY_END
    ADD R4,R4,R4        ; Left move the mask
    ADD R2,R2,#1        ; Increment the counter
    ADD R6,R2,#-5
    BRn DAY             ; Enumerate all 5 bits
    ADD R0,R0,#2        ; Point to next event
    LDR R6,R0,#0
    BRnp EVENT          ; empty string -> translation part ends
    AND R0,R0,#0
TRANSLATION_END         ; Load register value
    LD R1,TRA_R1
    LD R2,TRA_R2
    LD R3,TRA_R3
    LD R4,TRA_R4
    LD R5,TRA_R5
    LD R6,TRA_R6
    LD R7,TRA_R7
    RET    
SUB_INVALID             ; Raise Error
    ST  R1,WRONG_EVENT
    AND R0,R0,#0
    ADD R0,R0,#1
    BR  TRANSLATION_END
SUB_OCCUPY
    ST  R1,WRONG_EVENT
    AND R0,R0,#0
    ADD R0,R0,#-1
    BR  TRANSLATION_END
    
TRA_R0      .BLKW #1
TRA_R1      .BLKW #1
TRA_R2      .BLKW #1
TRA_R3      .BLKW #1
TRA_R4      .BLKW #1
TRA_R5      .BLKW #1
TRA_R6      .BLKW #1
TRA_R7      .BLKW #1
WRONG_EVENT .BLKW #1
DATA        .FILL x5000
TRA_MASK    .FILL x0001
TRANS       .FILL x4000


; PRINTING subroutine
; Function: Print the translated schedule to the screen
; Side Effect: None
; Register Usage: R0: Used for OUT and PUTS
;                 R1: Pass parameter to subroutines
;                 R2: Counter for the 5 days loop
;                 R3: Pointer to the translation list
;                 R5: Counter for the 15 hours loop
;                 R6: Temporary use for branch judgement
;                 R7: Reserved for TRAP
PRINTING
    ST R0,PRI_R0        ; Save register value
    ST R1,PRI_R1
    ST R2,PRI_R2
    ST R3,PRI_R3
    ST R5,PRI_R5
    ST R6,PRI_R6
    ST R7,PRI_R7
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
    LD  R3,CONTENT      ; Initialize translated list pointer
    LD  R5,PRI_MINUS    ; Initialize time counter
SCHEDULE
    ADD R5,R5,#1        ; Increment time counter
    ADD R1,R5,#0
    JSR PRINT_SLOT
    LD  R2,PRI_MINUS    ; Initialize day counter
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
    LD R0,PRI_R0        ; Load register value
    LD R1,PRI_R1
    LD R2,PRI_R2
    LD R3,PRI_R3
    LD R5,PRI_R5
    LD R6,PRI_R6
    LD R7,PRI_R7
    RET

PRI_R0      .BLKW #1
PRI_R1      .BLKW #1
PRI_R2      .BLKW #1
PRI_R3      .BLKW #1
PRI_R4      .BLKW #1
PRI_R5      .BLKW #1
PRI_R6      .BLKW #1
PRI_R7      .BLKW #1
PRI_MINUS   .FILL #-1
CONTENT     .FILL x4000
BAR         .FILL x007C
LINEFEED    .FILL x000A
BLANK       .STRINGZ ""
HEADER      .STRINGZ "Mon"
            .STRINGZ "Tue"
            .STRINGZ "Wed"
            .STRINGZ "Thu"
            .STRINGZ "Fri"


; INSERT subroutine
; Function: Attempting to find a compatible combination of times at which extra 
;           events can be inserted into an existing weekly schedule. Finally,
;           inserting valid timeslot combination into translation list at x4000.
; Output: R0 - Indicate failure or success of the subroutines 
;         (0: Success; 1: Can't fit into the schedule)
; Side Effect: R0 would be modified
; Stack degisn: Each event is assigned 6 memory addresses in stack
;               M[R6] contains current valid timeslot, which invalid time bits are all set to 0
;               M[R6-1] to M[R6-5] contains traslated address (if the event happens on that day)
;                               or contains x0000 (if the event doesn't happen on that day)
;               Intuitive demonstration:
;               Addr         Meaning              Content
;               R6-5        (Friday)           x40?? / x0000
;               R6-4       (Thursday)          x40?? / x0000
;               R6-3       (Wednesday)         x40?? / x0000
;               R6-2        (Tuesday)          x40?? / x0000
;               R6-1        (Monday)           x40?? / x0000
;               R6   ->   Valid Timeslot   A bit vectors of hours
INSERTING
    ST  R1,INS_R1       ;Save register value
    ST  R2,INS_R2
    ST  R3,INS_R3
    ST  R4,INS_R4
    ST  R5,INS_R5
    ST  R6,INS_R6
    ST  R7,INS_R7
    ; Register Usage for main loops (time loop and day loop): 
    ; R0: Extra list pointer
    ; R1: Day Counter
    ; R2: Time Counter
    ; R3: Day mask
    ; R4: Time mask
    ; R5: Load event Day or Time
    ; R6: Stack pointer
    ; R7: Temporary use
    LD  R0,EXTRA
    LD  R6,STACK
INS_MAIN_INIT
    LDR R7,R0,#2
    STR R7,R6,#0        ; PUSH ORIGINAL TIMESLOT
    BRz EMPTY_TIME
INS_BACK_INIT
    AND R2,R2,#0        ; Time loop initialize
    LD  R4,INS_MASK
INS_TIME
    LDR R5,R6,#0        ; Load timeslot
    NOT R7,R4
    AND R7,R5,R7        ; Set current time bit to 0
    STR R7,R6,#0        ; PUSH VALID TIMESLOT
    AND R7,R5,R4
    BRz INS_TIME_NEXT
    ADD R7,R2,R2
    ADD R7,R7,R7
    ADD R7,R7,R2        ; R7 = 5*R2
    ST  R7,TIME_NUM
    AND R1,R1,#0        ; Day loop initialze
    LD  R3,INS_MASK
INS_DAY                 ; Check days for valid time
    LDR R5,R0,#1
    AND R7,R5,R3
    BRnp INS_DAY_CHECK
    AND R5,R5,#0        ; No event: R5 = x0000
INS_DAY_NEXT
    ADD R3,R3,R3        ; Left move day mask
    NOT R7,R1
    ADD R7,R6,R7
    STR R5,R7,#0        ; PUSH x40?? or x0000
    ADD R1,R1,#1
    ADD R7,R1,#-5       ; Test the end of the week
    BRn INS_DAY
    LD  R5,STACK_BIAS   ; Whole week passed: Insect into translated list
INSERT
    ADD R1,R5,R6
    LDR R7,R1,#0        ; R7: Translated address
    BRz INSECT_NEXT
    LDR R1,R0,#0
    STR R1,R7,#0        ; Insect event label
INSECT_NEXT
    ADD R5,R5,#1
    BRn INSERT
EVENT_NEXT
    ADD R6,R6,#-6       ; Increment stack pointer
    ADD R0,R0,#3        ; Increment extra list pointer
    LDR R7,R0,#0        ; Test the end of list
    BRnp INS_MAIN_INIT
    AND R0,R0,#0        ; Set Successful flag
    BR  INSERTING_END   ; Subroutine ended
INS_DAY_CHECK
    LD  R7,TIME_NUM
    LD  R5,FIXED
    ADD R5,R5,R7        ; Add time bias
    ADD R5,R5,R1        ; Add day bias
    LDR R7,R5,#0        ; Exist event: R5 = Translated address
    BRz INS_DAY_NEXT
INS_TIME_NEXT           ; M[R5]!=0 -> Conflicts, check another time
    ADD R4,R4,R4        ; Left move the mask
    ADD R2,R2,#1        ; Increment time pointer
    ADD R7,R2,#-15      ; Check the end of timeslot
    BRnz INS_TIME
    ; BACKTRACKING: Forward move pointer & Check underflow & Clear inserted label
    ADD R0,R0,#-3       ; Forward move extra list pointer
    ADD R6,R6,#6        ; Forward move stack pointer
    LD  R7,STACK
    NOT R7,R7
    ADD R7,R7,#1
    ADD R7,R7,R6        ; If R6 > x8000, all posibility exhausted, raise error
    BRp SUB_NONE
    ; Function: Clear orinigally inserted address to zero
    ; Register Usage for clear loop: (All regiseters except R0 & R6 are reusable)
    ; R1: Counter & bias accumulator
    ; R2: Stack inner frame pointer
    ; R3: Translated address pointer
    ; R5: Clear value (x0000) contanier
    ; R7: Temporary Use
    AND R1,R1,#0
    AND R5,R5,#0
CLEAR_LOOP
    ADD R1,R1,#-1
    ADD R2,R6,R1
    LDR R3,R2,#0
    STR R5,R3,#0        ; Clear target memory
    ADD R7,R1,#5
    BRp CLEAR_LOOP
    BR  INS_BACK_INIT
EMPTY_TIME
    ; Function: For empty time, clear its all 6 stack memory
    ; Register Usage for empty_time loop: (R2,R4,R5,R7 are usable)
    ; R2: Counter & bias accumulator
    ; R5: Clear value (x0000) contanier
    ; R7: Translated address pointer & Temporary Use for branch
    AND R5,R5,#0
    ADD R2,R2,#-1
    ADD R7,R6,R2
    STR R5,R7,#0        ; Clear target memory
    ADD R7,R2,#5
    BRp EMPTY_TIME
    BR  EVENT_NEXT
SUB_NONE                ; Raise Error
    AND R0,R0,#0
    ADD R0,R0,#1        ; Set unsuccessful flag
INSERTING_END           ; Load register value
    LD  R1,INS_R1
    LD  R2,INS_R2
    LD  R3,INS_R3
    LD  R4,INS_R4
    LD  R5,INS_R5
    LD  R6,INS_R6
    LD  R7,INS_R7
    RET

INS_R0      .BLKW #1
INS_R1      .BLKW #1
INS_R2      .BLKW #1
INS_R3      .BLKW #1
INS_R4      .BLKW #1
INS_R5      .BLKW #1
INS_R6      .BLKW #1
INS_R7      .BLKW #1
TIME_NUM    .BLKW #1
STACK_BIAS  .FILL #-5
FIXED       .FILL x4000
EXTRA       .FILL x6000
STACK       .FILL x8000
INS_MASK    .FILL x0001


; PRINT_SLOT subroutine: Print specific time slot to screen
; Input: R1 (A number from 0-15)
; Output: Print "R1+7:00" to screen
; Side Effect: None
; Register Usage: R0: Store value loaded from the memory & Store the value to be printed
;                 R1: Parameter passed from main routine, indicating the time-7
;                 R6: Temporary used for branch judgement
;                 R7: Reserved for TRAP instruction
PRINT_SLOT
    ST R0,SLO_R0        ; Save register value
    ST R1,SLO_R1
    ST R6,SLO_R6
    ST R7,SLO_R7
    ; Main subroutine    
    LD  R0,ZERO
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
    LD R0,SLO_R0        ; Load register value
    LD R1,SLO_R1
    LD R6,SLO_R6
    LD R7,SLO_R7
    RET
    
SLO_R0      .BLKW   #1
SLO_R1      .BLKW   #1
SLO_R6      .BLKW   #1
SLO_R7      .BLKW   #1
BIAS        .FILL   x003A
ZERO        .FILL   x0030


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
    ST R0,CEN_R0        ; Save register value
    ST R1,CEN_R1
    ST R2,CEN_R2
    ST R3,CEN_R3
    ST R4,CEN_R4
    ST R6,CEN_R6
    ST R7,CEN_R7
    LD  R2,PC_MINUS     ; Initialize counter
COUNT
    ADD R2,R2,#1        ; Increment counter
    ADD R4,R1,R2        ; Compute address
    LDR R0,R4,#0
    BRnp COUNT          ; M[R4]!=0x00 -> string doesn't end
    ADD R6,R2,#-6
    BRn SHORT           ; Branch to code for string shorter than 6
                        ; Code for string longer than 6
    LD  R3,PC_MINUS     ; Initialize counter
MAIN_LONG
    ADD R3,R3,#1        ; Increment the counter
    ADD R4,R1,R3        ; Compute the address
    LDR R0,R4,#0
    OUT                 ; Print the character
    ADD R6,R3,#-5
    BRn MAIN_LONG       ; Enumerate all 6 characters
    BRnzp DONE
SHORT                   ; Code for string shorter than 6
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
    LD R0,CEN_R0        ; Load register value
    LD R1,CEN_R1
    LD R2,CEN_R2
    LD R3,CEN_R3
    LD R4,CEN_R4
    LD R6,CEN_R6
    LD R7,CEN_R7
    RET                 

CEN_R0      .BLKW   #1
CEN_R1      .BLKW   #1
CEN_R2      .BLKW   #1
CEN_R3      .BLKW   #1
CEN_R4      .BLKW   #1
CEN_R6      .BLKW   #1
CEN_R7      .BLKW   #1
SPACE       .FILL   x0020
PC_MINUS    .FILL   #-1

    .END
