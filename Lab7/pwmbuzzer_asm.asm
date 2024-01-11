;******************************************************************************
; Lab 4 starter code
; ECE 266, fall 2023
;
; Assembly functions related to buzzer
;
; Created by Zhao Zhang
;******************************************************************************

        ; To include names declared in C
        .cdecls "pwmbuzzer.h"

        .text

;******************************************************************************
; Buzzer signal pin usage: Grove base port J17, Tiva C PC5 (Port C, Pin 5)
;******************************************************************************

; Declare a field to store GPIO_PORTC_BASE (32-bit value) in program memory
_GPIO_PORTC_BASE    .field  GPIO_PORTC_BASE
WTIMER0				.field	WTIMER0_BASE
PIN_ROUTE 			.field	GPIO_PC5_WT0CCP1
PORT				.field	GPIO_PORTC_BASE
_PERIPH_WTIMER0 	.field 	SYSCTL_PERIPH_WTIMER0
_PERIPH_GPIOC		.field 	SYSCTL_PERIPH_GPIOC


;******************************************************************************
; C prototype: void buzzOn()
; Turn on the buzzer. It calls GPIOPinWrite() to write 1 to the signal pin.
;******************************************************************************
      	 .global BuzPwmInit
      	 .asmfunc
BuzPwmInit
 		PUSH    {LR}            ; save the return address


  		LDR     r0, _PERIPH_WTIMER0
   		BL      SysCtlPeripheralEnable

   		LDR     r0, _PERIPH_GPIOC
   		BL      SysCtlPeripheralEnable

    	LDR		r0, _GPIO_PORTC_BASE
     	MOV		r1, #GPIO_PIN_5
      	BL		GPIOPinTypeTimer

       	LDR		r0, PIN_ROUTE
        BL		GPIOPinConfigure

       	LDR		r0, WTIMER0
      	MOV		r2, #TIMER_CFG_SPLIT_PAIR
      	ORR		r1, r2, #TIMER_CFG_B_PWM
       	BL 		TimerConfigure


       	LDR		r0, WTIMER0
       	MOV		r1, #TIMER_B
       	MOV		r2, #1
       	BL		TimerControlLevel

       	LDR		r0, WTIMER0
      	MOV		r1, #TIMER_B
      	BL		TimerEnable

       POP     {PC}            ; return
       .endasmfunc


;******************************************************************************
; C prototype: void buzzOff()
; Turn off the buzzer. It calls GPIOPinWrite() to write 0 to the signal pin.
;******************************************************************************
        .global BuzPwmSet
       .asmfunc
BuzPwmSet
	PUSH    {lr, r1, r0}

	; call TmerLoadSet(
	LDR	r0, WTIMER0 ; r0 is our timer0
	MOV	r1, #TIMER_B ; r1 is our timer_b
	LDR r2, [sp, #0] ;
	BL  TimerLoadSet ; Call the timerloadset

	LDR	r0, WTIMER0
	MOV	r1, #TIMER_B
	LDR r2, [sp, #4]
	BL TimerMatchSet

	ADD		sp, #8
	POP     {PC}
       	.endasmfunc
