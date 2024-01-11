/*
 * motion.c
 *
 *  Created on: Sep 26, 2023
 *      Author: Lab group 6
 */


#include "motion.h"
#include "launchpad.h"
#include <driverlib/rom_map.h>
#include <driverlib/rom.h>
#include <driverlib/gpio.h>

/******************************************************************************
 * Initialize the motion sensor
 * Pin usage: Grove base port J16, Tiva C PC4 (Port C, Pin 4)
 *****************************************************************************/
typedef struct {
    uint32_t debouncing_delay;
    uint32_t pin_value;
    bool new_input;
} MotionSensorState;

static MotionSensorState motionsensor = {150, 0, false};

// pre-declare the ISR function
static void MotionSensorISR();

void MotionSensorInit()
{
    // Enable Port C peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    // Configure PC4 as input pin
    GPIOPinTypeGPIOInput(GPIO_PORTC_BASE, GPIO_PIN_4);

    GPIOIntRegister(GPIO_PORTC_BASE, MotionSensorISR); // register the interrupt handler
    GPIOIntTypeSet(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_BOTH_EDGES);
    IntPrioritySet(INT_GPIOC, 1);
    GPIOIntEnable(GPIO_PORTC_BASE, GPIO_PIN_4);

}

#define DEBOUNCE_TIME   400

int MotionSensorDetect()
{
    // If there is no new input, return -1
    if (!motionsensor.new_input)


    // Reset new input flag
    motionsensor.new_input = false;

    // Convert raw input to input code and return it
    if (motionsensor.pin_value != 0) {


        return 1;       // Motion detected

    } else {
        return 0;       // No motion detected

    }
}

static void MotionSensorISR()
{
    static uint32_t last_event_time = 0;      // remember last pushing time

    // Read Port F. SW1 and SW2 are active low, so invert the reading
    uint32_t pin_value = GPIOPinRead(GPIO_PORTC_BASE, GPIO_PIN_4);

    // De-bouncing: If a key was pushed within 250 ms, ignore this event
    uint32_t current_time = SysTimeGet();
    if (current_time > last_event_time + DEBOUNCE_TIME) {
        motionsensor.pin_value = abs(pin_value);
        motionsensor.new_input = true;
        last_event_time = current_time;
        uprintf("%d\r\n",pin_value);
    }

    // IMPORTANT: Clear interrupt flag
    GPIOIntClear(GPIO_PORTC_BASE, GPIO_PIN_4);
}

