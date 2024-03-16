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
#include "fonts/opensans8.h"
#include "fonts/opensans10.h"
#include "fonts/opensans8bold.h"
#include "fonts/opensans10bold.h"
#include "fonts/opensans12bold.h"
#include "fonts/opensans14bold.h"
#include "fonts/opensans16bold.h"
#include "fonts/opensans18bold.h"
#include "fonts/opensans24bold.h"

#endif //WEATHER_STATION_INCLUDE_H
