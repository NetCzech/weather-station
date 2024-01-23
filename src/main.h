#ifndef WEATHER_STATION_MAIN_H
#define WEATHER_STATION_MAIN_H

#include "include.h"

// Define the width and height of the display
#define SCREEN_WIDTH   EPD_WIDTH
#define SCREEN_HEIGHT  EPD_HEIGHT

// Define text alignment
enum alignment {LEFT, RIGHT, CENTER};

// Define graphic element variables
#define autoscale_on  true
#define autoscale_off false
#define barchart_on   true
#define barchart_off  false

// Define icon display variables
boolean LargeIcon   = true;
boolean SmallIcon   = false;

#define Large  20
#define Small  8

// Define time and date format
String  Time_str = "--:--:--";
String  Date_str = "-- --- ----";

// Define other variables
int     wifi_signal, CurrentHour = 0, CurrentMin = 0, CurrentSec = 0, EventCnt = 0, vref = 1100;

// *******************************
// PROGRAM VARIABLES AND OBJECTS
// *******************************

// Define the number of days for data retention (limited to 3 days here, but could go to 5 days = 40)
#define max_readings 24

Forecast_record_type  WxConditions[1];
Forecast_record_type  WxForecast[max_readings];

// Define a floating point field for meterology data
float pressure_readings[max_readings]    = {0};
float temperature_readings[max_readings] = {0};
float humidity_readings[max_readings]    = {0};
float rain_readings[max_readings]        = {0};
float snow_readings[max_readings]        = {0};

// Sleep time in minutes (aligned to the nearest minute boundary, so if 30 will always update at 00 or 30 past the hour)
long SleepDuration   = 60;
// Set the wake up time (save battery power)
int  WakeupHour      = 6;
// Set sleep time (save battery power)
int  SleepHour       = 23;
long StartTime       = 0;
long SleepTimer      = 0;
// ESP32 RTC speed compensation (prevents display at xx:59:yy and then xx:00:yy (one minute later), save battery power)
long Delta           = 30; // 

#endif //WEATHER_STATION_MAIN_H