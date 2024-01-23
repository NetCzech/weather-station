#ifndef WEATHER_STATION_INCLUDE_H
#define WEATHER_STATION_INCLUDE_H

#include <esp_task_wdt.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "epd_driver.h"
#include "esp_adc_cal.h"

#include <ArduinoJson.h>
#include <HTTPClient.h>

#include <WiFi.h>
#include <SPI.h>
#include <time.h>

#include "forecast_record.h"
#include "lily_draw.h"
#include "secret.h"
#include "lang.h"
#include "main.h"

// Images
#include "img/sunrise.h"
#include "img/sunset.h"

// Fonts
#include "fonts/OpenSans6.h"
#include "fonts/OpenSans8B.h"
#include "fonts/OpenSans10.h"
#include "fonts/OpenSans10B.h"
#include "fonts/OpenSans12B.h"
#include "fonts/OpenSans14B.h"
#include "fonts/OpenSans16B.h"
#include "fonts/OpenSans18B.h"
#include "fonts/OpenSans24B.h"

#endif //WEATHER_STATION_INCLUDE_H
