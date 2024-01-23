#ifndef SECRET_H
#define SECRET_H

#include "include.h"

const bool DebugDisplayUpdate = false;

// Wi-Fi access data
const char* ssid     = "your_ssid";
const char* password = "your_wifi_pass";

/* *************************************************************************************************************************************************************************

ENTER THE YOUR OPENWEATHER API KEY (register a free developer account at https://openweathermap.org/)
http://api.openweathermap.org/data/2.5/forecast?q=Melksham,UK&APPID=your_OWM_API_key&mode=json&units=metric&cnt=40
http://api.openweathermap.org/data/2.5/weather?q=Melksham,UK&APPID=your_OWM_API_key&mode=json&units=metric&cnt=1

**************************************************************************************************************************************************************************** */
String apikey       = "your_api_key";
const char server[] = "api.openweathermap.org";


/* *************************************************************************************************************************************************************************

SET LOCATION ACCORDING TO OPENWEATHER:
Your city:              http://bulk.openweathermap.org/sample/
Your country code:      https://en.wikipedia.org/wiki/List_of_ISO_3166_country_codes
Your language code:     only the weather description is translated
                        examples    Arabic (AR), Czech (CZ), English (EN), Greek (EL), Persian(Farsi) (FA), Galician (GL), Hungarian (HU), Japanese (JA)
                                    Korean (KR), Latvian (LA), Lithuanian (LT), Macedonian (MK), Slovak (SK), Slovenian (SL), Vietnamese (VI)
Your hemisphere:        north or south
Your units code:        M = metric
                        I = imperial
Your timezone:          https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
Your NTP server:        https://www.ntppool.org/en/
GMT offset:             UK normal time is GMT, so GMT Offset is 0, for US (-5Hrs) is typically -18000, AU is typically (+8hrs) 28800
Day light offset:       In the UK DST is +1hr or 3600-secs, other countries may use 2hrs 7200 or 30-mins 1800 or 5.5hrs 19800 Ahead of GMT use + offset behind - offset

**************************************************************************************************************************************************************************** */
String City             = "your_city";
String Country          = "your_country_code";
String Language         = "your_language_code";                           
String Hemisphere       = "your_hemisphere";
String Units            = "your_units_code";
const char* Timezone    = "your_timezone";
const char* ntpServer   = "your_ntp_server";                                                          
int   gmtOffset_sec     = 0;
int  daylightOffset_sec = 3600;

#endif // SECRET_H