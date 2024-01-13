/*
 * pwmmotion.c
 *
 *  Created on: Oct 24, 2023
 *      Author: Jeffrey and Narain
 */

#include <sensor.h>

/*
 * Initialize the timer PWM functions connected to the buzzer.
 *    Buzzer:      PC5, WT0CCP1, Timer 0, sub-timer B
 */
typedef struct
{
    uint32_t rising_edge_time;
    uint32_t falling_edge_time;
    uint32_t data;
    uint32_t count;
    uint32_t diff;
    bool rising_edge_detected;
    bool cycle_done;
    bool new_data_ready;
} SensorState;

static SensorState HT_sensor = { 0, 0, 0, 0, 0, false, false, false };

void SensorInit()
{
    // Enable port and timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // configure the timer and the sub-timer for time capture
    TimerConfigure(WTIMER2_BASE,
                   (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_TIME_UP));

    // configure event to capture time for both edges
    TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);

    // register ISR and enable timer interrupt
    TimerIntRegister(WTIMER2_BASE, TIMER_B, SensorISR);
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPB_EVENT);

    // Enable the timer and sub-timer
    TimerEnable(WTIMER2_BASE, TIMER_B);

}

void SensorTriggerReading()
{
    // Clean the state for the sensor
    HT_sensor.count = 0;

    // Clear software flags for rising edge ******
    HT_sensor.rising_edge_detected = false;
    HT_sensor.new_data_ready = false;

    // Configure the pin type to GPIO output pin
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    // write 1 to the pin and wait 2us
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    WaitUs(10);

    // write 0 to the pin and wait 1ms
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    WaitUs(1000);

    // write 1 to the pin to finish the starting pulse;
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    WaitUs(30);

    // configure and route pin to timer
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);

    // clear timer interrupt status
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}

bool DataReady()
{
    return HT_sensor.new_data_ready;
}

uint32_t ReadRaw()
{
    HT_sensor.new_data_ready = false;
    HT_sensor.rising_edge_detected = false;
    return HT_sensor.data;
}

void SensorISR()
{
    uint32_t time_value = TimerValueGet(WTIMER2_BASE, TIMER_B);

    HT_sensor.count++;

    // Stage 1: skip events #1 to #3
    if (HT_sensor.count >= 1 && HT_sensor.count <= 3)
    {
    }

    // Stage 2: process data bits
    else if (HT_sensor.count >= 4 && HT_sensor.count <= 67)
    {
        if (!HT_sensor.rising_edge_detected)
        {
            HT_sensor.rising_edge_time = time_value;
            HT_sensor.rising_edge_detected = true;
        }
        else
        {
            HT_sensor.falling_edge_time = time_value;
            HT_sensor.rising_edge_detected = false;

            // The code for capturing rising edge and falling edge times
            uint32_t diff = (HT_sensor.falling_edge_time
                    - HT_sensor.rising_edge_time);

            if (diff >= 22 * 50 && diff <= 30 * 50)
            {
                HT_sensor.data = (HT_sensor.data << 1) | 0; // inputting binary

            }
            else if (diff >= 68 * 50 && diff <= 75 * 50)
            {
                HT_sensor.data = (HT_sensor.data << 1) | 1; // inputting binary

            }
        }
    }

    // Stage 3: skip 8 parity pulses
    else if (HT_sensor.count >= 68 && HT_sensor.count <= 83)
    {
        if (HT_sensor.count == 83)
        {
            HT_sensor.new_data_ready = true;
        }
    }

    // Clear timer interrupt flag
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}

