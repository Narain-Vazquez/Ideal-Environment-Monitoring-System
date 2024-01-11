/*
 * pwmbuzzer.h
 *
 *  Created on: Oct 17, 2023
 *      Author: alitt
 */

#ifndef PWMBUZZER_H_
#define PWMBUZZER_H_

#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>

/*
 * Initialize the timer PWM functions connected to the three sub-LEDs.
 */
void BuzPwmInit();

// Initialize the buzzer

/*
 * Set the LED parameters for the three sub-LEDs
 */
void BuzPwmSet(int pulsePeriod, int pulseWidthBuzzer);




#endif /* PWMBUZZER_H_ */
