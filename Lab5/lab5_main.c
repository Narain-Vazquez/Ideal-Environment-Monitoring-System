    /*
 * Lab 4 starter code
 * ECE 266, fall 2023
 *
 * lab4_main.c: Implement the tasks for a motion detection alarm.
 *
 * Created by Zhao Zhang
 *
 */

#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "launchpad.h"
#include "buzzer.h"
#include "motion.h"

/********************************************************************************
 * Global declarations
 *******************************************************************************/

// The data structure for the system state
typedef struct {
    // Activated state: if the system is activated or not
    bool activated;

    // Alert state: If the alert is on or off
    bool alerted;

    // Sound state: If the buzzer is turned on to make a sound or not
    // Note: When the system is activated and alerted, the buzzer is turned
    // on and off periodically
    bool buzzer_on;
} SysState;

// The system state
static SysState sys =
{
    false /* not activated */,
    false /* not alerted */,
    false /* buzzer not turned on */
};

/********************************************************************************
 * Task 1: If the system is activated and alerted, make the buzzer play
 * on/off sound. The buzzer will be repeatedly turned on and off.
 *
 * You may adjust the on and off times to change the sound pattern and intensity.
 *
 * Note: This is a callback function.
 *******************************************************************************/

void BuzzerPlay(uint32_t time)                  // the scheduled time
{
    uint32_t delay = 500;                       // default checking interval

     if(sys.activated)
     {
        if (sys.alerted)
        {
            // Turn buzzer on and off
            if (sys.buzzer_on)
            {
                // The buzzer is on: turn it off
                BuzzerOff();
                sys.buzzer_on = false;
                delay = 3000;                       // off for 2988 ms
            }
            else
            {
                // The buzzer is off: turn it on
                BuzzerOn();
                sys.buzzer_on = true;
                delay = 10;                         // on for 12 ms
            }
        }
        else
        {
            // Make sure the buzzer is turned off
            if (sys.buzzer_on)
            {
                BuzzerOff();
                sys.buzzer_on = false;
            }
        }
    }

    // schedule callback to buzzer play
     ScheduleCallback(BuzzerPlay, time + delay);
}

/********************************************************************************
 * Task 2: YOUR CODE, check the motion sensor
 *
 * Check the motion sensor input. If motion is detected, put the system
 * in the alerted state. Otherwise, put it in the non-alerted state.
 *
 * This is a callback function. In the next lab, you will re-write this function
 * to make it interrupt based.
 *******************************************************************************/

void CheckMotionSensor()
{
    int motionDetected = MotionSensorDetect();

        if (motionDetected == 0)
        {
            sys.alerted = false;
            uprintf("motion not detected\r\n");
            if (sys.activated)
            {
                LedTurnOnOff(false /* red */, false /* blue */, true /* green */);
            }
        }
        else
        {
            sys.alerted = true;
           uprintf("motion detected\r\n");
            if (sys.activated)
            {
                LedTurnOnOff(true /* red */, false /* blue */, false /* green */);
            }
        }
}

/********************************************************************************
 * Task 3: Check push buttons.
 *
 * In the starter code: Pushing SW1 activates the system and put it in the
 * alerted state. Pushing SW2 deactivates the system.
 *
 * IN YOUR SOLUTION CODE: Pushing SW1 activates the system. Pushing SW2
 * deactivates the system. If the system is activated, the motion sensor
 * input decides if the system is put in the alerted state or not.
 *
 * This function is called upon interrupts. It is NOT a callback function.
 *
 * YOU NEED TO REVISE THIS FUNCTION
 *
 *******************************************************************************/

void CheckPushButton()
{
    int motionDetected = MotionSensorDetect();
    int code = PbRead();        // read the pushbutton

    switch (code)
    {
    case 1:                     // SW1 is pushed
        if (!sys.activated)
        {
             // Activate the system
             sys.activated = true;
             if (motionDetected == 0) {
                 sys.alerted = false;
             }
             else {
                 sys.alerted = true;
             }

        }
        break;

    case 2:                     // SW2 is pushed
        if (sys.activated)
        {
            // De-activate the system and turn off LED
            sys.activated = false;
            sys.alerted = false;
            BuzzerOff();
            LedTurnOnOff(false /* red */, false /* blue */, false /* green */);
        }
        break;
    }
}


/********************************************************************************
 * The main function: Print out a message, schedule the first callback event(s),
 * and then run the callback scheduler.
 *******************************************************************************/

int main(void)
 {
    // Initialize the Tiva C LaunchPad and 7-seg
    LpInit();
    PbInit();
    BuzzerInit();
    MotionSensorInit();

    // Print out a message. See uprintf() in launchpad.h
    uprintf("%s\n\r", "Lab 5: Motion Detection Alarm");

    // Schedule first callback events
    //ScheduleCallback(MotionSensor, 1000);
    ScheduleCallback(BuzzerPlay, 1005);

    while (true)
    {
        // Wait for interrupt events (include callback events)
        asm("   wfi");

        // Check for pushbutton interrupt events
        CheckPushButton();
        CheckMotionSensor();

        // Run callback events
        ScheduleExecute();
    }
}
