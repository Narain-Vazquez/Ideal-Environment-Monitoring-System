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
#include "temp_sensor.h"
#include "light_sensor.h"

// The state of the 7-segment display. Initially it shows "00:00" with the colon turned on.
Seg7Display seg7 = { { 10, 1, 10, 10 }, false /* colon on */};

/*
 * Trigger the temperature sensor reading periodically
 */
void TempSensorSampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    TsTriggerSampling();


    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(TempSensorSampling, time + 4000);

}

/*
 * Check temperature sensor reading, print if ready
 */
void LightSensorSampling(uint32_t time)
{
    // Trigger ADC sampling for the temperature sensor
    LsTriggerSampling();


    // Schedule the next callback in 1000 milliseconds
    ScheduleCallback(LightSensorSampling, time + 8000);

}

void CheckNewTempReading()
{
    // If there is no new reading, return immediately
    if (!TsNewDataReady())
        return;

    // Get the current temperature reading and convert it to Celsius
    uint32_t raw_data = TsReadRaw();
    float temp_C = 147.5 - (75 * 3.3 * raw_data / 4096);
    float temp_F = ((temp_C) * (9) / (5) + (32));



    // Update the 7-segment display with temperature and light sensor readings
    seg7.colon_on = true;
    seg7.digit[0] = 10;
    seg7.digit[1] = (uint32_t)(temp_F * 10) % 10;
    seg7.digit[2] = (uint32_t)temp_F % 10;
    seg7.digit[3] = (uint32_t)temp_F / 10;

    Seg7Update(&seg7);

    // Print temperature and light sensor readings on the terminal
    uprintf("Temperature Sensor Reading: %u.%u F\r\n", (uint32_t)temp_F, (uint32_t)(temp_F * 10) % 10);
  //  uprintf("Temp in C %u.%u\r\n", (uint32_t)temp_C, (uint32_t)(temp_C * 10) % 10);

}

void CheckNewLSReading()
{
    if (!LsNewDataReady())
            return;
// Get the current light sensor reading
        uint32_t LSraw_data = LsReadRaw();
        uprintf("Light Sensor Reading: %u C\r\n", LSraw_data);
    // Update the 7-segment display with light sensor reading
    seg7.colon_on = false;
    seg7.digit[0] = (uint32_t)(LSraw_data) % 10;
    if ((uint32_t)(LSraw_data) > 9){
        seg7.digit[1] = ((uint32_t)(LSraw_data) / 10) % 10;
    }
    else{
        seg7.digit[1] = 10;
    }
    if ((uint32_t)(LSraw_data) > 99){
        seg7.digit[2] = ((uint32_t)(LSraw_data) / 100) % 10;

    }
    else{
        seg7.digit[2] = 10;
    }

    if ((uint32_t)(LSraw_data) > 999){
        seg7.digit[3] = (uint32_t)(LSraw_data) / 1000;
    }
        else{
            seg7.digit[3] = 10;

        }


    Seg7Update(&seg7);
}

/*
 * The main function: Initialize and the execute the callback scheduler loop
 */

int main(void)
{

    LpInit();
    TsInit();
    Seg7Init();
    LsInit();

    uprintf("%s\n\r", "Lab 6: Ambient Sensing (ADC)");

    // Schedule the first callback
    ScheduleCallback(TempSensorSampling, 1000);
    ScheduleCallback(LightSensorSampling, 1000);

    // Loop forever
    while (true)
    {
        // Wait for interrupt to happen
        __asm("    wfi");

        // Check new temperature reading
        CheckNewTempReading();
        CheckNewLSReading();


        // Call the callback scheduler
        ScheduleExecute();
    }
}
