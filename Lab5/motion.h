/*
 * motion.h
 *
 *  Created on: Sep 26, 2023
 *      Author: Lab group 6
 */

#ifndef MOTION_H_
#define MOTION_H_

#include <stdint.h>
#include <stdbool.h>
#include <inc/hw_memmap.h>
#include <inc/hw_gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <inc/hw_ints.h>
#include <driverlib/interrupt.h>

// Initialize PIR motion sensor
void MotionSensorInit();

// Detect Motion
int MotionSensorDetect();

#endif /* MOTION_H_ */
