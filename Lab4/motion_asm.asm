
;******************************************************************************

        ; To include names declared in C
        .cdecls "motion.h"

        .text

;******************************************************************************
; Buzzer signal pin usage: Grove base port J17, Tiva C PC4 (Port C, Pin 4)
;******************************************************************************

; Declare a field to store GPIO_PORTC_BASE (32-bit value) in program memory
_GPIO_PORTC_BASE     .field  GPIO_PORTC_BASE

;******************************************************************************
;																			  ;
;******************************************************************************

		.global MotionSensorDetect
        .asmfunc

MotionSensorDetect

        PUSH    {LR}            ; save the return address

        ; GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4)

		LDR     r0, _GPIO_PORTC_BASE
		MOV     r1, #GPIO_PIN_4
        BL      GPIOPinRead

        POP     {PC}            ; return
        .endasmfunc


;******************************************************************************
;																			  ;
;******************************************************************************

