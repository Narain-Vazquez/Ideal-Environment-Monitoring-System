/*
 * lab2_main.c, starter code
 *
 * ECE 266 Lab 2, Fall 2023
 *
 * Created by Zhao Zhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <driverlib/sysctl.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_i2c.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/i2c.h>
#include "seg7.h"
#include "launchpad.h"

/********************************************************************************
 * Global declarations
 *******************************************************************************/

// The state of the 7-segment display. See seg7.h for the definition of Seg7Display.
Seg7Display seg7 = { { 0, 0, 0, 0 }, false }; // initial state is "3210" with colon off

/********************************************************************************
 * Task 1: Update the clock.
 *
 * This is a callback function.
 *
 * The starter code shows "01:23" on 7-seg with a flashing colon. It also
 * shows the same on the terminal.
 *
 * REVISE THE FUNCTION TO MAKE IT RUN A CLOCK
 *
 *******************************************************************************/

void ClockUpdate(uint32_t time)
{
    // Update the colon_on state, from true to false, or false to true
    if (seg7.colon_on)
        seg7.colon_on = false;
    else
        seg7.colon_on = true;

    // YOUR CODE STARTS HERE
    if(seg7.colon_on){
           seg7.digit[0]++;
     if(seg7.digit[0] > 9){
         seg7.digit[0] = 0;
         seg7.digit[1]++;
     }
     if(seg7.digit[1] > 5){
         seg7.digit[1] = 0;
         seg7.digit[2]++;
     }
     if(seg7.digit[2] > 9){
         seg7.digit[2] = 0;
         seg7.digit[3]++;
     }
     if(seg7.digit[3] > 5){
         seg7.digit[0] = 0;
         seg7.digit[1] = 0;
         seg7.digit[2] = 0;
         seg7.digit[3] = 0;
     }
    }


    // Update the 7-segment
    Seg7Update(&seg7);

    // Also print the clock on terminal in format of "mm:ss" or "mm ss"
    if (seg7.colon_on)
        uprintf("\r%c%c:%c%c", '0' + seg7.digit[3], '0' + seg7.digit[2],
                '0' + seg7.digit[1], '0' + seg7.digit[0]);
    else
        uprintf("\r%c%c %c%c", '0' + seg7.digit[3], '0' + seg7.digit[2],
                '0' + seg7.digit[1], '0' + seg7.digit[0]);

    // Schedule a callback after 0.5 seconds
    ScheduleCallback(ClockUpdate, time + 500);
}

/********************************************************************************
 * Task 2: Check push button events. If SW1 is pushed, advance minutes by 1.
 * If SW is pushed, reset seconds to 00.
 *
 * This is an interrupt-processing function.
 *******************************************************************************/

void CheckPushButton()
{
    // YOUR CODE STARTS HERE
    // See the starter code for Lab 1 for a template for this function
    // Read the pushbutton state; see pbRead() in launchpad.h
    int input = PbRead();

    switch (input)
    {
    case 1:
        // Switch to next color setting, with wrap-around
        seg7.digit[2]++;
        break;

    case 2:
        seg7.digit[0] = 0;
        seg7.digit[1] = 0;
        break;
    }
}

/********************************************************************************
 * The main function: Print out a message, schedule the first callback event(s),
 * and then run the callback scheduler.
 *******************************************************************************/

int main(void)
{
    // Initialize the Tiva C LaunchPad, including the clock rate, systick, GPIO,
    // UART0, pushbutton, and callback event scheduler
    LpInit();
    Seg7Init();

    // Print out a message. See uprintf() in launchpad.h
    uprintf("\n\r");
    uprintf("%s\n\r", "Lab 2: Wall clock");

    // Schedule the first callback events for FlashLED
    ScheduleCallback(ClockUpdate, 1000);

    // Run the callback scheduler
    // See schdExecute() in launchpad.h
    while (true)
    {
        // Wait for interrupt events (include callback events)
        asm("   wfi");

        // Check for pushbutton interrupt events
        CheckPushButton();

        // Run callback events
        ScheduleExecute();
    }
}
