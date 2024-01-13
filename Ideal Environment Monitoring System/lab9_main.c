/*
 * Starter code for ECE 266 Lab 9, main.c, Fall 2023
 *
 * Lab 9: Ideal Environment Monitoring System (Open-Ended Project)
 *
 * Created by Jeffrey and Narain
 */

#include <sensor.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "launchpad.h"
#include "light_sensor.h"
#include "sensor.h"
#include "temp_sensor.h"
#include "buzzer.h"

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

int lightsense;
int tempsense;
int humidsense;

// The state of the 7-segment display. Initially it shows "00:00" with the colon turned on.

/*
 * Trigger the temperature sensor reading periodically
 */
typedef enum
{
    Off, On
} OnOff;

typedef struct
{
    bool red;               // Red sub-LED on or off
    bool blue;              // Blue sub-LED on or off
    bool green;             // Green sub-LED on or off
} LedColor;

#define LED_COLOR_NUM       3
LedColor led_colors[LED_COLOR_NUM] = { { true, false, false }, // Red on, Blue off, Green off
        { false, true, false },   // Red off, Blue on, Green off
        { false, false, true }    // Red off, Blue off, Green on
};
OnOff led_state = Off;
int color_choice = 0;

uint32_t humidity_data = 0;
uint32_t temperature_data = 0;



bool test;
// The system state

void FlashLED(uint32_t time)                // the scheduled time for this event
{
    LedColor color = led_colors[color_choice];     // current color setting

    switch (led_state)
    {
    case Off:
        // Turn on LED by setting the sub-LED colors, change the LED state to On
        // See ledTurnOfOff() in launchpad.h
        LedTurnOnOff(color.red, color.blue, color.green);
        led_state = On;
        break;

    case On:
        // Turn off all sub-LEDs, change the LED state to Off
        // See ledTurnOnOff() in launchpad.h
        LedTurnOnOff(false, false, false);
        led_state = Off;
        break;
    }

    // Schedule the next callback event with a delay of 1000 ms from the current time
    // ScheduleCallback() in launchpad.h
    ScheduleCallback(FlashLED, time + 1000);
}

/*void Sampling(uint32_t time)
 {
 // Trigger ADC sampling for the temperature sensor
 RangerTriggerReading();

 // Schedule the next callback in 1000 milliseconds
 ScheduleCallback(Sampling, time + 1000);

 }*/

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


void TempSensorSampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    TsTriggerSampling();


    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(TempSensorSampling, time + 1000);

}

void LightSensorSampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    LsTriggerSampling();

    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(LightSensorSampling, time + 1000);

}

void CheckNewLSReading()
{
    if (!LsNewDataReady())
        return;
	
   	// Get the current light sensor reading
    uint32_t LSraw_data = LsReadRaw();
    uprintf("Light Sensor Reading: %u\r\n", LSraw_data);

	// Determining whether it’s in an ideal state
    if (LSraw_data < 900)
    {
        lightsense = 0;
    }

    if (LSraw_data >= 900)
    {
        lightsense = 1;
    }

}
void CheckNewTempReading()
{
    // If there is no new reading, return immediately
    if (!TsNewDataReady())
        return;

    // Get the current temperature reading and convert it to Celsius
    uint32_t raw_data = TsReadRaw();
    float temp_C = 147.5 - (75 * 3.3 * raw_data / 4096);

    if(temp_C > 22.5){
        tempsense = 0;
    }
    else{
        tempsense = 1;
    }
  //  uprintf("Temp in C %u\r\n", (uint32_t)temp_C);

    // Print temperature and light sensor readings on the terminal
  //  uprintf("Temperature Sensor Reading: %u.%u F\r\n", (uint32_t)temp_F, (uint32_t)(temp_F * 10) % 10);
    uprintf("Temp in C %u.%u\r\n", (uint32_t)temp_C, (uint32_t)(temp_C * 10) % 10);

}

void Sampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    SensorTriggerReading();

    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(Sampling, time + 1000);

}

void CheckNewHTReading()
{
    if (DataReady())
    {
	// getting raw data (40 bits)
        uint32_t raw_data = ReadRaw();
	// checking raw data
        uprintf("Sensor Reading: %x \r\n", raw_data);
	
	// humidity reading
        humidity_data = (raw_data >> 16) / 10;

        uprintf("Humidity Reading: %u RH \r\n", humidity_data);
	
	// temperature reading (not used, to slow for real time data)	
        temperature_data = ((raw_data << 16) >> 16) /10;

        //uprintf("Temperature Reading: %u C \r\n", temperature_data);
	
	// Determining whether it's in an ideal state
        if(humidity_data > 40){
            humidsense = 0;
        }
        else{
            humidsense = 1;
        }
        if (humidsense && tempsense && lightsense > 0){
            sys.activated = false;
            color_choice = 2;
            BuzzerOff();
        }
        else{
            sys.activated = true;
            color_choice = 0;
            BuzzerOn();
        }
    }
    else{
        return;
    }

}
/*
 * The main function: Initialize and the execute the callback scheduler loop
 */

int main(void)
    {
    LpInit();
    SensorInit();
    LsInit();
    BuzzerInit();
    TsInit();

    uprintf("%s\n\r", "Lab 9: Ideal Environment Monitoring System");

    ScheduleCallback(FlashLED, 1000);
    ScheduleCallback(LightSensorSampling, 1000);
    ScheduleCallback(Sampling, 1000);
    ScheduleCallback(TempSensorSampling, 1000);
    ScheduleCallback(BuzzerPlay, 1005);

    // Loop forever
    while (true)
    {
        // Wait for interrupt to happen
        __asm(" wfi");

        // Check new temperature reading
        CheckNewLSReading();
        CheckNewHTReading();
        CheckNewTempReading();
        // Call the callback scheduler
        ScheduleExecute();
    }
}



