/*
 * Starter code for ECE 266 Lab 6, main.c, fall 2023
 *
 * Lab 6: Ambient Sensing (ADC)
 *
 * Created by Zhao Zhang
 */

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "seg7.h"
#include "ranger.h"
// yo

// The state of the 7-segment display. Initially it shows "00:00" with the colon turned on.
Seg7Display seg7 = { { 1, 1, 1, 1 }, false /* colon on */};

/*
 * Trigger the temperaturea sensor reading periodically
 */
typedef struct {

    bool activated;
    bool feet_inches;
    float feet_inches_value;
    float millimeters_value;

} SysState;
bool test;
// The system state
static SysState sys =
{
    false /* not activated */,
    true /* not alerted */,
    0.0,
    0.0,
};

void Sampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    RangerTriggerReading();

    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(Sampling, time + 1000);

}

void CheckNewReading()
{

        if (DataReady()){

            uint32_t raw_data = ReadRaw();
            float millimeters = (((raw_data/50000000.0)/2)*340)*1000;

            uprintf("Ranger Sensor Reading (mm): %f \r\n", (float)millimeters );
            sys.millimeters_value = millimeters;

            float feet = ((((raw_data/50000000.0)/2)*340)*1000 / 304.8);
            sys.feet_inches_value = feet;

            uprintf("Ranger Sensor Reading (ft): %f \r\n", (float)feet );

            uint32_t foot = feet;
            uint32_t inches = ((feet - foot) * 12);

            if(sys.activated){
                if(sys.feet_inches){

                    // feet and inches reading
                    seg7.colon_on = true;

                    if (9 >= inches >= 1){
                        seg7.digit[0] = inches;
                        seg7.digit[1] = 10;
                    }
                    else if (inches > 9){
                        seg7.digit[0] = inches % 10;
                        seg7.digit[1] = inches / 10;
                    }
                    else{                   // (inches < 1)
                        seg7.digit[0] = 0;
                    }


                    if (9 >= foot >= 1){
                        seg7.digit[2] = foot;
                        seg7.digit[3] = 10;

                    }
                    else if (foot > 9){
                        seg7.digit[2] = foot % 10;
                        seg7.digit[3] = foot / 10;
                    }
                    else{                   // (foot == 0){
                        seg7.digit[2] = 0;
                        seg7.digit[3] = 10;
                    }
                }
                else{
                    // millimeter reading
                    seg7.colon_on = false;
                    if (millimeters > 0){
                            seg7.digit[0] = (uint32_t)(millimeters) % 10;
                    }
                    else{
                        seg7.digit[0] = 0;
                    }
                    if ( millimeters > 9){
                              seg7.digit[1] = (uint32_t)(millimeters / 10) % 10;
                    }
                    else{
                        seg7.digit[1] = 10;
                    }
                    if (millimeters > 99){
                              seg7.digit[2] = (uint32_t)(millimeters / 100) % 10;
                    }
                    else{
                        seg7.digit[2] = 10;
                    }
                    if (millimeters > 999){
                              seg7.digit[3] = (uint32_t)(millimeters / 1000);
                    }
                    else{
                        seg7.digit[3] = 10;
                    }

                }
            }
            else{
                // turn off 7seg display
                seg7.digit[0] = 10;
                seg7.digit[1] = 10;
                seg7.digit[2] = 10;
                seg7.digit[3] = 10;
            }

            Seg7Update(&seg7);

            return;
        }
        else{
            return;
        }
}


void CheckPushButtons()
{
    // Process the pushbutton input
    switch (PbRead())
    {
    case 1:
        if(!sys.activated)
        {
        sys.activated = true;
        seg7.colon_on = true;

        }
        else{
            sys.activated = false;
            seg7.colon_on = false;
        }
        break;

    case 2:

        if(sys.feet_inches)
        {
            sys.feet_inches = false;
        }
        else{
            sys.feet_inches = true;
        }

        break;
    }
}
/*
 * The main function: Initialize and the execute the callback scheduler loop
 */

int main(void)
{
    LpInit();
    Seg7Init();
    RangerInit();


    uprintf("%s\n\r", "Lab 8: Ranger");

    ScheduleCallback(Sampling, 1000);

    // Loop forever
    while (true)
    {
        // Wait for interrupt to happen
        __asm(" wfi");

        // Check new temperature reading
        CheckPushButtons();
        CheckNewReading();

        // Call the callback scheduler
        ScheduleExecute();
    }
}

