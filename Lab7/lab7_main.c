/*
 * Starter code for ECE 266 Lab 7, Sound Player (PWM), fall 2023
 *
 * main.c
 *
 * Created by Zhao Zhang
 */

// REVISE AS NEEDED
#include "launchpad.h"
#include "pwmled.h"
#include "pwmbuzzer.h"

/*
 * Definition, data types and global variables
 *
 * REVISE AS NEEDED
 */

#define CPU_CLOCK_RATE      50000000
#define NUM_PITCH           7
#define NUM_VOLUME_LEVEL    3

// Color weight settings defined as the ratio of red, blue, and green colors
typedef struct
{
    float red;
    float blue;
    float green;
} LEDColorSetting;

static LEDColorSetting led_color[NUM_PITCH] =
{
    {0.33, 0.33, 0.33},
    {1.0, 0.0, 0.0},
    {0.5, 0.5, 0.0},
    {0.0, 1.0, 0.0},
    {0.0, 0.5, 0.5},
    {0.0, 0.0, 1.0},
    {0.5, 0.0, 0.5},
};

// LED brightness settings defined as PWM period and PWM pulse width
typedef struct
{
    int period;
    int pulse_width;
} LEDBrightnessSetting;

static LEDBrightnessSetting led_brightness[NUM_VOLUME_LEVEL] =
{
   {5000, 10},
   {5000, 20},
   {5000, 100}
};

// System state
typedef struct
{
    int sound_pitch;            // choice of sound pitch
    int sound_volume;           // choice of sound volume
    bool buzzer_on;             // if buzzer is currently turned on
} SysState;

static SysState sys_state = { 0, 0, false };
//
//
//
typedef struct
{
    float note;
} MusicNotes;

static MusicNotes music_notes[NUM_PITCH] =
{
   {261.63},
   {293.66},
   {329.63},
   {349.23},
   {392.00},
   {440.00},
   {493.88}
};
//
//
//
typedef struct
{
    int period;
    int pulse_width;
} BuzzerVolumeSetting;

static BuzzerVolumeSetting buzzer_volume[NUM_VOLUME_LEVEL] =
{
     {5000, 10},
    {5000, 50},
    {5000, 70}
};
/*
 * Set LED color and brightness using PWM
 */
void SetLED()
{
    int pulse_width_red;
    int pulse_width_blue;
    int pulse_width_green;

    // Get the current color and brightness setting
    LEDColorSetting color = led_color[sys_state.sound_pitch];
    LEDBrightnessSetting brightness = led_brightness[sys_state.sound_volume];

    // Calculate PWM pulse widths for red, blue, and green LEDs
    if (sys_state.buzzer_on) {
        pulse_width_red = brightness.pulse_width * color.red;
        pulse_width_blue = brightness.pulse_width * color.blue;
        pulse_width_green = brightness.pulse_width * color.green;
    } else {
        // if brightness_level == -1, pulse width will be 0 to turn off LEDs
        pulse_width_red = 0;
        pulse_width_blue = 0;
        pulse_width_green = 0;
    }

    LedPwmSet(brightness.period, pulse_width_red, pulse_width_blue, pulse_width_green);
}
//
//
//
void SetBuzzer()
{
    int period_buzzer;
    int pulse_width_buzzer;

    // Use the correct struct names and members
    MusicNotes music = music_notes[sys_state.sound_pitch];
    BuzzerVolumeSetting volume = buzzer_volume[sys_state.sound_volume];

    if (sys_state.buzzer_on) {
        period_buzzer =  CPU_CLOCK_RATE / music.note;
        pulse_width_buzzer =  period_buzzer * .01 * volume.pulse_width;
    } else {
        pulse_width_buzzer = 0;
    }

    BuzPwmSet(period_buzzer, pulse_width_buzzer);
}
/*
 * Task 1: Check the push buttons.
 *
 * Use interrupt-based push button input.
 */
void CheckPushButtons()
{
    // Process the pushbutton input
    switch (PbRead())
    {
    case 1:
        sys_state.sound_pitch = (sys_state.sound_pitch + 1) % NUM_PITCH;
        break;

    case 2:
        sys_state.sound_volume = (sys_state.sound_volume + 1) % NUM_VOLUME_LEVEL;
        break;
    }
}

/*
 * Task 2: Play the buzzer (callback function)
 *
 * The buzzer and LEDs will be turned on for 0.5s, off for 1.0s, and so forth.
 *
 * NOTE: THE CURRENT VERSION ONLY FLASHES LED. IT DOES NOT YET PLAY SOUND.
 */
void PlaySound(uint32_t time)
{
    int delay;

    if (!sys_state.buzzer_on) {
        // Turn on the buzzer with the current PWM setting
        SetLED();
        SetBuzzer();
        //SetBuzzer();

        sys_state.buzzer_on = true;
        delay = 500;
    }
    else
    {
        // Turn off the buzzer
        SetLED();
        SetBuzzer();
        //SetBuzzer();
        sys_state.buzzer_on = false;
        delay = 1000;
    }

    // Increment the position and schedule the next callback
    ScheduleCallback(PlaySound, time + delay);
}

/*
 * The main function
 */
void main(void)
{
    LpInit();
    LedPwmInit();
    BuzPwmInit();


    ScheduleCallback(PlaySound, 1000);

    // Loop forever
    while (true)
    {
        // Wait for interrupt
        asm("   wfi");

        // Check push button events
        CheckPushButtons();

        // Execute scheduled callbacks
        ScheduleExecute();
    }
}
