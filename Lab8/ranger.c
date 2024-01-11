/*
 * pwmmotion.c
 *
 *  Created on: Oct 24, 2023
 *      Author: vazqu
 */

#include "ranger.h"

/*
 * Initialize the timer PWM functions connected to the buzzer.
 *    Buzzer:      PC5, WT0CCP1, Timer 0, sub-timer B
 */
typedef struct {
    uint32_t time_value;
    uint32_t rising_edge_time;
    uint32_t falling_edge_time;
    bool rising_edge_input;
    bool new_data_ready;
} RangerState;

static RangerState ranger_sensor = { 0, 0, 0, false, false};

void RangerInit()
{
    // Enable port and timer
    SysCtlPeripheralEnable(SYSCTL_PERIPH_WTIMER2);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

    // configure the timer and the sub-timer for time capture
    TimerConfigure(WTIMER2_BASE, (TIMER_CFG_SPLIT_PAIR | TIMER_CFG_B_CAP_TIME_UP));

    // configure event to capture time for both edges
    TimerControlEvent(WTIMER2_BASE, TIMER_B, TIMER_EVENT_BOTH_EDGES);

    // register ISR and enable timer interrupt
    TimerIntRegister(WTIMER2_BASE, TIMER_B, RangerISR);
    TimerIntEnable(WTIMER2_BASE, TIMER_CAPB_EVENT);

    // Enable the timer and sub-timer
    TimerEnable(WTIMER2_BASE, TIMER_B);

}

void RangerTriggerReading()
{
    // Clear software flags for rising edge ******
    ranger_sensor.rising_edge_input = false;
    ranger_sensor.new_data_ready = false;

    // Configure the pin type to GPIO output pin
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE, GPIO_PIN_1);

    // write 0 to the pin and wait 2us
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);
    WaitUs(2);

    // write 1 to the pin and wait 5us
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_PIN_1);
    WaitUs(5);

    // write 0 to the pin to finish the starting pulse;
    GPIOPinWrite(GPIO_PORTD_BASE, GPIO_PIN_1, 0);

    // configure and route pin to timer
    GPIOPinTypeTimer(GPIO_PORTD_BASE, GPIO_PIN_1);
    GPIOPinConfigure(GPIO_PD1_WT2CCP1);

    // clear timer interrupt status
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);

}

bool DataReady()
{
    return ranger_sensor.new_data_ready;
}

uint32_t ReadRaw()
{
    ranger_sensor.new_data_ready = false;
    ranger_sensor.rising_edge_input = false;
    return (ranger_sensor.falling_edge_time - ranger_sensor.rising_edge_time);
}

void RangerISR()
{
    uint32_t time_value = TimerValueGet(WTIMER2_BASE, TIMER_B);

    if (!ranger_sensor.rising_edge_input) {
        ranger_sensor.rising_edge_time = time_value;
        ranger_sensor.rising_edge_input = true;
    }
    else {
        ranger_sensor.falling_edge_time = time_value;
        ranger_sensor.new_data_ready = true;
    }

    // Clear timer interrupt flag
    TimerIntClear(WTIMER2_BASE, TIMER_CAPB_EVENT);
}



