/*
 * light_sensor.h
 *
 *  Created on: Oct 10, 2023
 *      Author: Jeffrey and Narain
 */

#ifndef LIGHT_SENSOR_H_
#define LIGHT_SENSOR_H_

#include <stdbool.h>
#include <stdint.h>

void LsInit();

// Trigger temperature reading
void LsTriggerSampling();

// Check if any new reading is ready
bool LsNewDataReady();

// Read the most recent temperature reading
uint32_t LsReadRaw();

#endif /* LIGHT_SENSOR_H_ */
