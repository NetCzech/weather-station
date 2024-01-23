#ifndef LANG_H
#define LANG_H

#include "include.h"

#define FONT(x) x##_tf

//Temperature - Humidity - Forecast
const String TXT_FORECAST_VALUES  = "3 denni predpoved";
const String TXT_CONDITIONS       = "Podminky";
const String TXT_DAYS             = "[dny]";
const String TXT_TEMPERATURES     = "Teplota";
const String TXT_TEMPERATURE_C    = "Teplota [°C]";
const String TXT_TEMPERATURE_F    = "Teplota [°F]";
const String TXT_HUMIDITY_PERCENT = "Vlhkost [%]";

// Pressure
const String TXT_PRESSURE         = "Tlak";
const String TXT_PRESSURE_HPA     = "Tlak [hPa]";
const String TXT_PRESSURE_IN      = "Tlak [in]";
const String TXT_PRESSURE_STEADY  = "Ustalene";
const String TXT_PRESSURE_RISING  = "Stoupajici";
const String TXT_PRESSURE_FALLING = "Klesajici";

//RainFall / SnowFall
const String TXT_RAINFALL_MM = "Srazky [mm]";
const String TXT_RAINFALL_IN = "Srazky [in]";
const String TXT_SNOWFALL_MM = "Snezeni [mm]";
const String TXT_SNOWFALL_IN = "Snezeni [in]";
const String TXT_PRECIPITATION_SOON = "Prec.";


//Sun
const String TXT_SUNRISE  = "Vychod slunce ";
const String TXT_SUNSET   = "Zapad slunce ";

//Moon
const String TXT_MOON_NEW             = "Nov";
const String TXT_MOON_WAXING_CRESCENT = "Dorustajici srpek";
const String TXT_MOON_FIRST_QUARTER   = "Prvni ctvrtina";
const String TXT_MOON_WAXING_GIBBOUS  = "Dorustajici mesic";
const String TXT_MOON_FULL            = "Uplnek";
const String TXT_MOON_WANING_GIBBOUS  = "Couvajici mesic";
const String TXT_MOON_THIRD_QUARTER   = "Posledni ctvrtina";
const String TXT_MOON_WANING_CRESCENT = "Couvajici srpek";

//Power / WiFi
const String TXT_POWER  = "Napajeni";
const String TXT_WIFI   = "Wi-Fi";
const char* TXT_UPDATED = "Aktualizace:";


//Wind
const String TXT_WIND_SPEED_DIRECTION = "Rychlost/smer vetru";
const String TXT_N   = "S";
const String TXT_NNE = "SSV";
const String TXT_NE  = "SV";
const String TXT_ENE = "VSV";
const String TXT_E   = "V";
const String TXT_ESE = "VJV";
const String TXT_SE  = "JV";
const String TXT_SSE = "JJV";
const String TXT_S   = "J";
const String TXT_SSW = "JJZ";
const String TXT_SW  = "JZ";
const String TXT_WSW = "ZJZ";
const String TXT_W   = "Z";
const String TXT_WNW = "ZSZ";
const String TXT_NW  = "SZ";
const String TXT_NNW = "SSZ";

//Day of the week
const char* weekday_D[] = {"nedele", "pondeli", "utery", "streda", "ctvrtek", "patek", "sobota"};

//Month
const char* month_M[] = {"leden", "unor", "brezen", "duben", "kveten", "cerven", "cervenec", "srpen", "zari", "rijen", "listopad", "prosinec"};

#endif // LANG_H