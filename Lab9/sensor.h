/*
 * ranger.h
 *
 *  Created on: Oct 29, 2023
 *      Author: vazqu
 */

#ifndef SENSOR_H_
#define SENSOR_H_

#include <stdbool.h>
#include <stdint.h>
#include <inc/hw_memmap.h>
#include <driverlib/sysctl.h>
#include <driverlib/gpio.h>
#include <driverlib/pin_map.h>
#include <driverlib/timer.h>
#include <sys/types.h>
#include <inc/hw_ints.h>
#include "launchpad.h"


#include <stdio.h>
#include <driverlib/interrupt.h>
#include <driverlib/adc.h>

/*
 * Initialize
 */
void SensorInit();

void SensorTriggerReading();

bool DataReady();

uint32_t ReadRaw();

void SensorISR();




#endif /* SENSOR_H_ */
