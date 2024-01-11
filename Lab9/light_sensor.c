/*
 * light_sensor.c
 *
 *  Created on: Oct 10, 2023
 *      Author:
 */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_ints.h>
#include <driverlib/sysctl.h>
#include <driverlib/interrupt.h>
#include <driverlib/adc.h>
#include <light_sensor.h>

typedef struct {
    uint32_t raw_data;
    bool new_data_ready;
}LightSensorState;

static LightSensorState light_sensor = {0, false};

static void LightSensorISR();

void LsInit()
{
    // Enable the ADC0 peripheral
    SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);

    // Configure ADC0's sequencer #1
    ADCSequenceConfigure(ADC0_BASE, 1 /* sequencer */, ADC_TRIGGER_PROCESSOR, 0 /* priority */);

    // Configure step 0 of sequencer 0 to use the temperature sensor, with
    // interrupt enable, as the end of the sequence
    ADCSequenceStepConfigure(ADC0_BASE, 1 /* sequencer */, 0 /* step */,
                             ADC_CTL_CH7 | ADC_CTL_IE | ADC_CTL_END);

    // Enable ADC0, sequencer 0
    ADCSequenceEnable(ADC0_BASE, 1 /* sequencer */);

    // Configure the interrupt system for temperature sensor
    ADCIntRegister(ADC0_BASE, 1 /* sequencer */, LightSensorISR);
    ADCIntEnable(ADC0_BASE, 1 /* sequencer */);
}

void LsTriggerSampling()
{
    // Trigger the sampling
    ADCProcessorTrigger(ADC0_BASE, 1);
}

bool LsNewDataReady()
{
    return light_sensor.new_data_ready;
}

uint32_t LsReadRaw()
{
    // Reset new data ready flag and return the reading
    light_sensor.new_data_ready = false;
    return light_sensor.raw_data;
}

static void LightSensorISR()
{
    // Read the ADC data
    ADCSequenceDataGet(ADC0_BASE, 1 /* sequencer */,
                       &light_sensor.raw_data /* pointer to data buffer */);

    // Set the flag for new data ready
    light_sensor.new_data_ready = true;

    // IMPORTANT: Clear the interrupt flag
    ADCIntClear(ADC0_BASE, 1 /* sequencer */);
}
