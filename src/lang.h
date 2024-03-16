#ifndef LANG_H
#define LANG_H

#include "include.h"

#define FONT(x) x##_tf

//Temperature - Humidity - Forecast
const String TXT_FORECAST_VALUES  = "3 denní předpověď";
const String TXT_CONDITIONS       = "Podmínky";
const String TXT_DAYS             = "[dny]";
const String TXT_TEMPERATURES     = "Teplota";
const String TXT_TEMPERATURE_C    = "Teplota [°C]";
const String TXT_TEMPERATURE_F    = "Teplota [°F]";
const String TXT_HUMIDITY_PERCENT = "Vlhkost [%]";

// Pressure
const String TXT_PRESSURE         = "Tlak";
const String TXT_PRESSURE_HPA     = "Tlak [hPa]";
const String TXT_PRESSURE_IN      = "Tlak [in]";
const String TXT_PRESSURE_STEADY  = "Ustálené";
const String TXT_PRESSURE_RISING  = "Stoupající";
const String TXT_PRESSURE_FALLING = "Klesající";

//RainFall / SnowFall
const String TXT_RAINFALL_MM = "Srážky [mm]";
const String TXT_RAINFALL_IN = "Srážky [in]";
const String TXT_SNOWFALL_MM = "Sněžení [mm]";
const String TXT_SNOWFALL_IN = "Sněžení [in]";
const String TXT_PRECIPITATION_SOON = "Prec.";


//Sun
const String TXT_SUNRISE  = "Východ slunce ";
const String TXT_SUNSET   = "Západ slunce ";

//Moon
const String TXT_MOON_NEW             = "Nov";
const String TXT_MOON_WAXING_CRESCENT = "Dorůstající srpek";
const String TXT_MOON_FIRST_QUARTER   = "První čtvrtina";
const String TXT_MOON_WAXING_GIBBOUS  = "Dorůstající měsíc";
const String TXT_MOON_FULL            = "Úplněk";
const String TXT_MOON_WANING_GIBBOUS  = "Couvající měsíc";
const String TXT_MOON_THIRD_QUARTER   = "Poslední čtvrtina";
const String TXT_MOON_WANING_CRESCENT = "Couvající srpek";

//Power / WiFi
const String TXT_POWER  = "Napájení";
const String TXT_WIFI   = "Wi-Fi";
const char* TXT_UPDATED = "Aktualizace:";


//Wind
const String TXT_WIND_SPEED_DIRECTION = "Rychlost/směr větru";
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
const char* weekday_D[] = {"neděle", "pondělí", "úterý", "středa", "čtvrtek", "pátek", "sobota"};

//Month
const char* month_M[] = {"leden", "únor", "březen", "duben", "květen", "červen", "červenec", "srpen", "září", "říjen", "listopad", "prosinec"};

#endif // LANG_H