;
; clockupdate.asm: ECE 266 Lab 3 Starter code, fall 2023
;
; Assembly code for updating the 7-seg to show a running clock
;
; Created by Zhao Zhang
;

; include C header file
        .cdecls "clockupdate.h"
        .text

; Declaration for a pointer to external variable seg7
seg7_ptr        .field  seg7
ClockUpdate_ptr .field  ClockUpdate

;************************************************************************************
; Task 1: Update the clock
;
; C prototype: void ClockUpdate(uint32_t time)
;
; This is the ClockUpdate callback function in assembly. It replaces the
; same function in your lab2_main.c.
;
; This is the STARTER CODE. At this time it only flashes the colon of the 7-seg.
; Complete the code so that it updates all the four clock digits.
;************************************************************************************
        .global ClockUpdate
        .asmfunc
ClockUpdate     
        PUSH    {lr, r0}            ; push return address and time

        ; load the address of seg7 to r0
        LDR     r0, seg7_ptr

        ; check seg7.colon_on and turn it on/off
        LDRB    r1, [r0, #4]        ; r1 = seg7.colon_on
        CMP     r1, #0              ; compare r1 and 0
        BEQ     turn_on_colon       ; if equal, jump to turn on the colon
        MOV     r1, #0              ; r1 = 0, for turning off the colon
        STRB    r1, [r0, #4]        ; seg7.colon_on = 0
        B       update_seg7_and_return   ; jump to return

turn_on_colon
        MOV     r1, #1              ; r1 = 1, for turning on the colon
        STRB    r1, [r0, #4]        ; seg7.colon_on = 1

        ;**************************************************************
        ; YOUR CODE STARTS HERE
        ;**************************************************************

		; Increment the first digit and check if it reaches 10
	    LDRB    r1, [r0, #0]         	; r1 = seg7.digit[0]
	    ADD     r1, #1               	; seg7.digit[0]++
	    CMP     r1, #10              	; compare seg7.digit[0] and 10
	    BEQ     reset_and_incr_digit_0 	; if equal, jump to reset and increment the next digit
	    STRB    r1, [r0, #0]         	; if not equal to 10, move the value of seg7.digit[0] to the memory so clockupdate can read it
	    B       update_seg7_and_return 	; jump to return

reset_and_incr_digit_0
	    MOV     r1, #0               	; make seg7.digit[0] =0,
	    STRB    r1, [r0, #0]         	; store that value into memory so clock update can read it
	    LDRB    r1, [r0, #1]         	; r1 = seg7.digit[1]
	    ADD     r1, #1               	; seg7.digit[1]++
	    CMP     r1, #6               	; compare seg7.digit[1] and 6
	    BEQ     reset_and_incr_digit_1 	; if equal, jump to reset and increment the next digit
	    STRB    r1, [r0, #1]           	; if not turn print out the current value, by storing the value into memory
	    B       update_seg7_and_return 	; jump to return

reset_and_incr_digit_1
	    MOV     r1, #0               	; make seg7.digit[1] =0
	    STRB    r1, [r0, #1]         	; store that value into memory so clock update can read it
	    LDRB    r1, [r0, #2]         	; r1 = seg7.digit[2]
	    ADD     r1, #1               	; seg7.digit[2]++
	    CMP     r1, #10              	; compare seg7.digit[2] and 10
	    BEQ     reset_and_incr_digit_2 	; if equal, jump to reset and increment the next digit
	    STRB    r1, [r0, #2]         	; if not turn print out the current value, by storing the value into memory
	    B       update_seg7_and_return 	; jump to return

reset_and_incr_digit_2
	    MOV     r1, #0               	; make seg7.digit[2] =0
	    STRB    r1, [r0, #2]         	; store that value into memory so clock update can read it
	    LDRB    r1, [r0, #3]         	; r1 = seg7.digit[3]
	    ADD     r1, #1               	; seg7.digit[3]++
	    CMP     r1, #6               	; compare seg7.digit[3] and 6
	    BEQ     start_clock_again    	; if equal, jump to reset the entire clock
	    STRB    r1, [r0, #3]         	; if not turn print out the current value, by storing the value into memory
	    B       update_seg7_and_return 	; jump to return

start_clock_again
	    MOV     r1, #0              	; reset every display
        STRB    r1, [r0, #0]
	    MOV     r1, #0
        STRB    r1, [r0, #1]
        MOV     r1, #0
        STRB    r1, [r0, #2]
        MOV     r1, #0
        STRB    r1, [r0, #3]

        ;**************************************************************
        ; YOUR CODE ENDS HERE
        ;**************************************************************

update_seg7_and_return
        ; Physically update the 7-seg
        ;    Call Seg7Update(&seg)
        BL      Seg7Update          ; note that r0 = &seg7 at this time

        ; Schedule a callback after 0.5 seconds:
        ;   Call ScheduleCallback(ClockUpdate, time + 500);
        LDR     r0, ClockUpdate_ptr ; r0 = ClockUpdate
        POP     {r1}                ; r1 = time
        ADD     r1, #500            ; r1 += 500
        BL      ScheduleCallback

        POP     {pc}                ; return
        .endasmfunc
