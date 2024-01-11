/*
 * Starter code for ECE 266 Lab 7, Sound Player (PWM), fall 2023
 *
 * pwmled.c: PWM LED functions
 *
 * Created by Zhao Zhang
 */

#include "pwmbuzzer.h"

/*
 * Initialize the timer PWM functions connected to the buzzer.
 *    Buzzer:      PC5, WT0CCP1, Timer 0, sub-timer B
 */


void BuzPwmInit()
{
    // Enable Timer 0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER0);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Connect pins to those timers
    GPIOPinTypeTimer(GPIO_PORTC_BASE, GPIO_PIN_5);
    GPIOPinConfigure(GPIO_PC5_WT0CCP1);

    // Select PWM for Timer 0 B
    TimerConfigure(WTIMER0_BASE,(TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_PWM));

    // Invert the PWM waveform, so that the Match register value is the pulse width.
    // Otherwise, the pulse width will be (Load value) - (Match value).
    // The inversion is done by enabling output inversion on the PWM pins.
    TimerControlLevel(WTIMER0_BASE, TIMER_B, true /* output inversion */);

    // Enable the Timer 0's TimerB and Timer 1's TimerA and TimerB
    TimerEnable(WTIMER0_BASE, TIMER_B);
}


void BuzPwmSet(int pulse_period, int pulse_width_buzzzer)
{
    // Set the PWM parameters for buzzer
    TimerLoadSet(WTIMER0_BASE, TIMER_B, pulse_period - 1);
    TimerMatchSet(WTIMER0_BASE, TIMER_B, pulse_width_buzzzer - 1);

}



