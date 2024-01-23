// ***********************************************************************************
// 
//                         Main code for weather station
//
// ***********************************************************************************
//
//                          This code is a fork of
//  https://github.com/Xinyuan-LilyGO/LilyGo-EPD-4-7-OWM-Weather-Display/tree/main
//
//       This software, the ideas and concepts is Copyright (c) David Bird 2021.
//                All rights to this software are reserved.
//
// ***********************************************************************************

#include <Arduino.h>
#include "include.h"

GFXfont  currentFont;
uint8_t *framebuffer;

// FORMAT AND UPDATE DATE
boolean UpdateLocalTime() {
  struct tm timeinfo;
  char date_output[30], time_output[30];
  while (!getLocalTime(&timeinfo, 5000)) {
    Serial.println("Failed to obtain time");
    return false;
  }
  CurrentHour = timeinfo.tm_hour;
  CurrentMin = timeinfo.tm_min;
  CurrentSec = timeinfo.tm_sec;

  if (Units == "M") {
    // Time in 24-hour format
    // Format: Day of Month, Month, Year, Day of Week, e.g., "31 May 2024, Saturday"
    sprintf(date_output, "%02u. %s %04u, %s", timeinfo.tm_mday, month_M[timeinfo.tm_mon], (timeinfo.tm_year) + 1900, weekday_D[timeinfo.tm_wday]);
    // To display the full time format change to "strftime(time_output, sizeof(time_output), "%H", &timeinfo);"
    sprintf(time_output, "%02d", timeinfo.tm_hour);
  } else {
    // Time in 12-hour format with AM/PM
    // Format: Day of Month, Month, Year, Day of Week, e.g., "May 31 2024, Saturday"
    strftime(date_output, sizeof(date_output), "%b %d %Y, %a", &timeinfo);
    // To display the full time format change to "strftime(time_output, sizeof(time_output), "%r", &timeinfo);"
    strftime(time_output, sizeof(time_output), "%I %p", &timeinfo);
  }

  Date_str = date_output;
  Time_str = time_output;
  return true;
}

// TIME SETTING AND SYNCHRONIZATION
boolean SetupTime() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer, "time.nist.gov");
  setenv("TZ", Timezone, 1);
  tzset();
  delay(100);
  return UpdateLocalTime();
}

// CONVERT UNIX TIME TO TIME AND DATE FORMAT
String ConvertUnixTime(int unix_time) {
  time_t tm = unix_time;
  struct tm *now_tm = localtime(&tm);
  char output[40];
  if (Units == "M") {
    // Format date and time for 24-hour format
    strftime(output, sizeof(output), "%H:%M %d/%m/%y", now_tm);
  }
  else {
    // Format date ant time for 12-hour format
    strftime(output, sizeof(output), "%I:%M%P %m/%d/%y", now_tm);
  }
  return output;
}

// CALCULATE THE DATE OF THE JULIAN CALENDAR
// A Julian date is the number of days that have passed since the beginning of the Julian period, which began in 4713 BC.
int JulianDate(int d, int m, int y) {
  int mm, yy, k1, k2, k3, j;
  yy = y - (int)((12 - m) / 10);
  mm = m + 9;
  if (mm >= 12) mm = mm - 12;
  k1 = (int)(365.25 * (yy + 4712));
  k2 = (int)(30.6001 * mm + 0.5);
  k3 = (int)((int)((yy / 100) + 49) * 0.75) - 38;
  // 'j' is the Julian date at 12h UT (Universal Time) For Gregorian calendar
  j = k1 + k2 + d + 59 + 1;
  if (j > 2299160) j = j - k3;
  return j;
}

// CONVERT WIND DIRECTION TO A TEXT STRING
String WindDegToOrdinalDirection(float winddirection) {
  if (winddirection >= 348.75 || winddirection < 11.25)  return TXT_N;
  if (winddirection >=  11.25 && winddirection < 33.75)  return TXT_NNE;
  if (winddirection >=  33.75 && winddirection < 56.25)  return TXT_NE;
  if (winddirection >=  56.25 && winddirection < 78.75)  return TXT_ENE;
  if (winddirection >=  78.75 && winddirection < 101.25) return TXT_E;
  if (winddirection >= 101.25 && winddirection < 123.75) return TXT_ESE;
  if (winddirection >= 123.75 && winddirection < 146.25) return TXT_SE;
  if (winddirection >= 146.25 && winddirection < 168.75) return TXT_SSE;
  if (winddirection >= 168.75 && winddirection < 191.25) return TXT_S;
  if (winddirection >= 191.25 && winddirection < 213.75) return TXT_SSW;
  if (winddirection >= 213.75 && winddirection < 236.25) return TXT_SW;
  if (winddirection >= 236.25 && winddirection < 258.75) return TXT_WSW;
  if (winddirection >= 258.75 && winddirection < 281.25) return TXT_W;
  if (winddirection >= 281.25 && winddirection < 303.75) return TXT_WNW;
  if (winddirection >= 303.75 && winddirection < 326.25) return TXT_NW;
  if (winddirection >= 326.25 && winddirection < 348.75) return TXT_NNW;
  return "?";
}

// CALCULATION OF THE PHASE OF THE MOON ACCORDING TO THE CURRENT DATE
String MoonPhase(int d, int m, int y, String hemisphere) {
  int c, e;
  double jd;
  int b;
  if (m < 3) {
    y--;
    m += 12;
  }
  ++m;
  c   = 365.25 * y;
  e   = 30.6  * m;
  // jd is total days elapsed
  jd  = c + e + d - 694039.09;
  // divide by the moon cycle (29.53 days)
  jd /= 29.53059;
  // int(jd) -> b, take integer part of jd
  b   = jd;
  // subtract integer part to leave fractional part of original jd
  jd -= b;
  // scale fraction from 0-8 and round by adding 0.5
  b   = jd * 8 + 0.5;
  // 0 and 8 are the same phase so modulo 8 for 0
  b   = b & 7;
  if (hemisphere == "south") b = 7 - b;
  // Moon phase New (0%  illuminated)
  if (b == 0) return TXT_MOON_NEW;
  // Moon phase Waxing crescent (25%  illuminated)
  if (b == 1) return TXT_MOON_WAXING_CRESCENT;
  // Moon phase First quarter (50%  illuminated)
  if (b == 2) return TXT_MOON_FIRST_QUARTER;
  // Moon phase Waxing gibbous (75%  illuminated)
  if (b == 3) return TXT_MOON_WAXING_GIBBOUS;
  // Moon phase Full (100% illuminated)
  if (b == 4) return TXT_MOON_FULL;
  // Moon phase Waning gibbous (75%  illuminated)
  if (b == 5) return TXT_MOON_WANING_GIBBOUS;
  // Moon phase Third quarter (50%  illuminated)
  if (b == 6) return TXT_MOON_THIRD_QUARTER;
  // Moon phase Waning crescent (25%  illuminated)
  if (b == 7) return TXT_MOON_WANING_CRESCENT;
  return "";
}

// EDIT THE TEXT TO "CAPITALIZE THE FIRST LETTER"
String TitleCase(String text) {
  if (text.length() > 0) {
    String temp_text = text.substring(0, 1);
    temp_text.toUpperCase();
    return temp_text + text.substring(1);
  }
  else return text;
}

// CONNECT TO WI-FI
uint8_t StartWiFi() {
  Serial.println("\r\nConnecting to: " + String(ssid));
  // Use Google DNS
  IPAddress dns(8, 8, 8, 8);
  WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  WiFi.setAutoConnect(true);
  WiFi.setAutoReconnect(true);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("STA: Failed!\n");
    WiFi.disconnect(false);
    delay(500);
    WiFi.begin(ssid, password);
  }
  if (WiFi.status() == WL_CONNECTED) {
    wifi_signal = WiFi.RSSI();
    Serial.println("WiFi connected at: " + WiFi.localIP().toString());
  }
  else Serial.println("WiFi connection *** FAILED ***");
  return WiFi.status();
}

// CONVERT mm TO INCHES
float mm_to_inches(float value_mm) {
  return 0.0393701 * value_mm;
}

// CONVERT hPa TO inHg
float hPa_to_inHg(float value_hPa) {
  return 0.02953 * value_hPa;
}

// CALCULATE THE SUN OF VALUES IN A FLOATING-POINT ARRAY
float SumOfPrecip(float DataArray[], int readings) {
  float sum = 0;
  for (int i = 0; i <= readings; i++) {
    sum += DataArray[i];
  }
  return sum;
}

// CALCULATE THE PHASE OF THE MOON FOR THE GIVEN DATE
double NormalizedMoonPhase(int d, int m, int y) {
  int j = JulianDate(d, m, y);
  //Calculate approximate moon phase
  double Phase = (j + 4.867) / 29.53059;
  return (Phase - (int) Phase);
}

// DRAW A TEXT STRING AND GRAPHIC ELEMENTS
void drawString(int x, int y, String text, alignment align) {
  const char * data  = const_cast<const char*>(text.c_str());
  int32_t  x1, y1;
  int32_t w, h;
  int32_t xx = x, yy = y;
  get_text_bounds(&currentFont, data, &xx, &yy, &x1, &y1, &w, &h, NULL);
    if (align == RIGHT)  x = x - w;
    if (align == CENTER) x = x - w / 2;
    int32_t cursor_y = y + h;
    int32_t x32 = x;
    write_string(&currentFont, data, &x32, &cursor_y, framebuffer);
}

// DRAW A GRAPHICAL WI-FI STRENGHT INDICATOR
void DrawRSSI(int x, int y, int rssi) {
  int WIFIsignal = 0;
  int xpos = 1;
  for (int _rssi = -100; _rssi <= rssi; _rssi = _rssi + 20) {
    // <-20dbm displays 5-bars
    if (_rssi <= -20)  WIFIsignal = 30;
    // -40dbm to  -21dbm displays 4-bars
    if (_rssi <= -40)  WIFIsignal = 24;
    // -60dbm to  -41dbm displays 3-bars
    if (_rssi <= -60)  WIFIsignal = 18;
    // -80dbm to  -61dbm displays 2-bars
    if (_rssi <= -80)  WIFIsignal = 12;
    // -100dbm to  -81dbm displays 1-bar
    if (_rssi <= -100) WIFIsignal = 6;
    // Set position on x-axis and y-axis
    fillRect(x + xpos * 8, y - WIFIsignal + 5 , 6, WIFIsignal, Black);
    xpos++;
  }
}

// DRAW BATTERY INDICATOR
void DrawBattery(int x, int y) {
  uint8_t percentage = 100;
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_value_t val_type = esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {
    Serial.printf("eFuse Vref:%u mV", adc_chars.vref);
    vref = adc_chars.vref;
  }
  float voltage = analogRead(36) / 4096.0 * 6.566 * (vref / 1000.0);
  if (voltage > 1 ) { // Only display if there is a valid reading
    Serial.println("\nVoltage = " + String(voltage));
    percentage = 2836.9625 * pow(voltage, 4) - 43987.4889 * pow(voltage, 3) + 255233.8134 * pow(voltage, 2) - 656689.7123 * voltage + 632041.7303;
    if (voltage >= 4.20) percentage = 100;
    if (voltage <= 3.20) percentage = 0;  // orig 3.5
    drawRect(x + 25, y - 14, 40, 15, Black);
    fillRect(x + 65, y - 10, 4, 7, Black);
    fillRect(x + 27, y - 12, 36 * percentage / 100.0, 11, Black);
    drawString(x + 85, y - 14, String(percentage) + "%  " + String(voltage, 1) + "v", LEFT);
  }
}

// DRAW GRAPHS
void DrawGraph(int x_pos, int y_pos, int gwidth, int gheight, float Y1Min, float Y1Max, String title, float DataArray[], int readings, boolean auto_scale, boolean barchart_mode) {
// Sets the autoscale increment, so axis steps up fter a change of e.g. 3
#define auto_scale_margin 0
// 5 Y-axis division markers
#define y_minor_axis 5
  setFont(OpenSans10B);
  int maxYscale = -10000;
  int minYscale =  10000;
  int last_x, last_y;
  int center_x = x_pos + gwidth / 2;
  float x2, y2;
  if (auto_scale == true) {
    for (int i = 1; i < readings; i++ ) {
      if (DataArray[i] >= maxYscale) maxYscale = DataArray[i];
      if (DataArray[i] <= minYscale) minYscale = DataArray[i];
    }
    // Auto scale the graph and round to the nearest value defined, default was Y1Max
    maxYscale = round(maxYscale + auto_scale_margin);
    Y1Max = round(maxYscale + 0.5);
    // Auto scale the graph and round to the nearest value defined, default was Y1Min
    if (minYscale != 0) minYscale = round(minYscale - auto_scale_margin);
    Y1Min = round(minYscale);
  }
  last_x = x_pos + 1;
  last_y = y_pos + (Y1Max - constrain(DataArray[1], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight;
  drawRect(x_pos, y_pos, gwidth + 3, gheight + 2, Grey);
  drawString(center_x, y_pos - 28, title, CENTER);
  for (int gx = 0; gx < readings; gx++) {
    // Max_readings is the global variable that sets the maximum data that can be plotted
    x2 = x_pos + gx * gwidth / (readings - 1) - 1 ;
    y2 = y_pos + (Y1Max - constrain(DataArray[gx], Y1Min, Y1Max)) / (Y1Max - Y1Min) * gheight + 1;
    if (barchart_mode) {
      fillRect(last_x + 2, y2, (gwidth / readings) - 1, y_pos + gheight - y2 + 2, Black);
    } else {
      // Two lines for hi-res display
      drawLine(last_x, last_y - 1, x2, y2 - 1, Black);
      drawLine(last_x, last_y, x2, y2, Black);
    }
    last_x = x2;
    last_y = y2;
  }
  //Draw the Y-axis scale
  #define number_of_dashes 20
  setFont(OpenSans);
  for (int spacing = 0; spacing <= y_minor_axis; spacing++) {
    // Draw dashed graph grid lines
    for (int j = 0; j < number_of_dashes; j++) {
      if (spacing < y_minor_axis) drawFastHLine((x_pos + 3 + j * gwidth / number_of_dashes), y_pos + (gheight * spacing / y_minor_axis), gwidth / (2 * number_of_dashes), Grey);
    }
    if ((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing) < 5 || title == TXT_PRESSURE_IN) {
      drawString(x_pos - 10, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 1), RIGHT);
    }
    else
    {
      if (Y1Min < 1 && Y1Max < 10) {
        drawString(x_pos - 3, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 1), RIGHT);
      }
      else {
        drawString(x_pos - 7, y_pos + gheight * spacing / y_minor_axis - 5, String((Y1Max - (float)(Y1Max - Y1Min) / y_minor_axis * spacing + 0.01), 0), RIGHT);
      }
    }
  }
  setFont(OpenSans8B);
  for (int i = 0; i < 3; i++) {
    drawString(20 + x_pos + gwidth / 3 * i, y_pos + gheight + 10, String(i) + "d", LEFT);
    if (i < 2) drawFastVLine(x_pos + gwidth / 3 * i + gwidth / 3, y_pos, gheight, LightGrey);
  }
}

// DISPLAY PRESSURE AND PRESSURE TREND
void DrawPressureAndTrend(int x, int y, float pressure, String slope) {
  drawString(x + 25, y - 10, String(pressure, (Units == "M" ? 0 : 1)) + (Units == "M" ? " hPa" : " in"), LEFT);
  if      (slope == "+") {
    DrawSegment(x, y, 0, 0, 8, -8, 8, -8, 16, 0);
    DrawSegment(x - 1, y, 0, 0, 8, -8, 8, -8, 16, 0);
  }
  else if (slope == "0") {
    DrawSegment(x, y, 8, -8, 16, 0, 8, 8, 16, 0);
    DrawSegment(x - 1, y, 8, -8, 16, 0, 8, 8, 16, 0);
  }
  else if (slope == "-") {
    DrawSegment(x, y, 0, 0, 8, 8, 8, 8, 16, 0);
    DrawSegment(x - 1, y, 0, 0, 8, 8, 8, 8, 16, 0);
  }
}

// DRAW THE CURRENT PHASE OF THE MOON
void DrawMoon(int x, int y, int dd, int mm, int yy, String hemisphere) {
  const int diameter = 75;
  double Phase = NormalizedMoonPhase(dd, mm, yy);
  hemisphere.toLowerCase();
  if (hemisphere == "south") Phase = 1 - Phase;
  // Draw dark part of moon
  fillCircle(x + diameter - 1, y + diameter, diameter / 2 + 1, Grey);
  const int number_of_lines = 90;
  for (double Ypos = 0; Ypos <= number_of_lines / 2; Ypos++) {
    double Xpos = sqrt(number_of_lines / 2 * number_of_lines / 2 - Ypos * Ypos);
    // Determine the edges of the lighted part of the moon
    double Rpos = 2 * Xpos;
    double Xpos1, Xpos2;
    if (Phase < 0.5) {
      Xpos1 = -Xpos;
      Xpos2 = Rpos - 2 * Phase * Rpos - Xpos;
    }
    else {
      Xpos1 = Xpos;
      Xpos2 = Xpos - 2 * Phase * Rpos + Rpos;
    }
    // Draw light part of moon
    double pW1x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
    double pW1y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
    double pW2x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
    double pW2y = (number_of_lines - Ypos)  / number_of_lines * diameter + y;
    double pW3x = (Xpos1 + number_of_lines) / number_of_lines * diameter + x;
    double pW3y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
    double pW4x = (Xpos2 + number_of_lines) / number_of_lines * diameter + x;
    double pW4y = (Ypos + number_of_lines)  / number_of_lines * diameter + y;
    drawLine(pW1x, pW1y, pW2x, pW2y, White);
    drawLine(pW3x, pW3y, pW4x, pW4y, White);
  }
  drawCircle(x + diameter - 1, y + diameter, diameter / 2, Black);
}

// DRAW SUNRISE IMAGE
void DrawSunrise(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = sunrise_width, .height =  sunrise_height
  };
  epd_draw_grayscale_image(area, (uint8_t *) sunrise_data);
}

// DRAW SUNSET IMAGE
void DrawSunset(int x, int y) {
  Rect_t area = {
    .x = x, .y = y, .width  = sunset_width, .height =  sunset_height
  };
  epd_draw_grayscale_image(area, (uint8_t *) sunset_data);
}

// CONVERT VALUES FROM METRIC TO IMPERIAL UNITS (only the first 3-hours are used)
void Convert_Readings_to_Imperial() {
  WxConditions[0].Pressure = hPa_to_inHg(WxConditions[0].Pressure);
  WxForecast[0].Rainfall   = mm_to_inches(WxForecast[0].Rainfall);
  WxForecast[0].Snowfall   = mm_to_inches(WxForecast[0].Snowfall);
}

// DECODE METEROLOGICAL DATA FROM JSON
bool DecodeWeather(WiFiClient& json, String Type) {
  Serial.print(F("\nCreating object...and "));
  // Allocate the JSON document
  DynamicJsonDocument doc(64 * 1024);
  // Deserialize the JSON document
  DeserializationError error = deserializeJson(doc, json);
  // Test if parsing succeeds
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.c_str());
    return false;
  }
  // Convert it to a JSON object
  JsonObject root = doc.as<JsonObject>();
  Serial.println(" Decoding " + Type + " data");
  // Current weather
  if (Type == "weather") {
    // All Serial.println statements are for diagnostic purposes and some are not required, remove if not needed with
    //WxConditions[0].lon         = root["coord"]["lon"].as<float>();              Serial.println(" Lon: " + String(WxConditions[0].lon));
    //WxConditions[0].lat         = root["coord"]["lat"].as<float>();              Serial.println(" Lat: " + String(WxConditions[0].lat));
    WxConditions[0].Main0       = root["weather"][0]["main"].as<const char*>();        Serial.println("Main: " + String(WxConditions[0].Main0));
    WxConditions[0].Forecast0   = root["weather"][0]["description"].as<const char*>(); Serial.println("For0: " + String(WxConditions[0].Forecast0));
    //WxConditions[0].Forecast1   = root["weather"][1]["description"].as<char*>(); Serial.println("For1: " + String(WxConditions[0].Forecast1));
    //WxConditions[0].Forecast2   = root["weather"][2]["description"].as<char*>(); Serial.println("For2: " + String(WxConditions[0].Forecast2));
    WxConditions[0].Icon        = root["weather"][0]["icon"].as<const char*>();        Serial.println("Icon: " + String(WxConditions[0].Icon));
    WxConditions[0].Temperature = root["main"]["temp"].as<float>();              Serial.println("Temp: " + String(WxConditions[0].Temperature));
    WxConditions[0].Pressure    = root["main"]["pressure"].as<float>();          Serial.println("Pres: " + String(WxConditions[0].Pressure));
    WxConditions[0].Humidity    = root["main"]["humidity"].as<float>();          Serial.println("Humi: " + String(WxConditions[0].Humidity));
    WxConditions[0].Low         = root["main"]["temp_min"].as<float>();          Serial.println("TLow: " + String(WxConditions[0].Low));
    WxConditions[0].High        = root["main"]["temp_max"].as<float>();          Serial.println("THig: " + String(WxConditions[0].High));
    WxConditions[0].Windspeed   = root["wind"]["speed"].as<float>();             Serial.println("WSpd: " + String(WxConditions[0].Windspeed));
    WxConditions[0].Winddir     = root["wind"]["deg"].as<float>();               Serial.println("WDir: " + String(WxConditions[0].Winddir));
    WxConditions[0].Cloudcover  = root["clouds"]["all"].as<int>();               Serial.println("CCov: " + String(WxConditions[0].Cloudcover)); // in % of cloud cover
    WxConditions[0].Visibility  = root["visibility"].as<int>();                  Serial.println("Visi: " + String(WxConditions[0].Visibility)); // in metres
    WxConditions[0].Rainfall    = root["rain"]["1h"].as<float>();                Serial.println("Rain: " + String(WxConditions[0].Rainfall));
    WxConditions[0].Snowfall    = root["snow"]["1h"].as<float>();                Serial.println("Snow: " + String(WxConditions[0].Snowfall));
    //WxConditions[0].Country     = root["sys"]["country"].as<char*>();            Serial.println("Ctry: " + String(WxConditions[0].Country));
    WxConditions[0].Sunrise     = root["sys"]["sunrise"].as<int>();              Serial.println("SRis: " + String(WxConditions[0].Sunrise));
    WxConditions[0].Sunset      = root["sys"]["sunset"].as<int>();               Serial.println("SSet: " + String(WxConditions[0].Sunset));
    WxConditions[0].Timezone    = root["timezone"].as<int>();                    Serial.println("TZon: " + String(WxConditions[0].Timezone));
  }
  // Weather forecast
  if (Type == "forecast") {
    //Serial.println(json);
    Serial.print(F("\nReceiving Forecast period - ")); //------------------------------------------------
    JsonArray list                  = root["list"];
    for (byte r = 0; r < max_readings; r++) {
      Serial.println("\nPeriod-" + String(r) + "--------------");
      WxForecast[r].Dt                = list[r]["dt"].as<int>();
      WxForecast[r].Temperature       = list[r]["main"]["temp"].as<float>();              Serial.println("Temp: " + String(WxForecast[r].Temperature));
      WxForecast[r].Low               = list[r]["main"]["temp_min"].as<float>();          Serial.println("TLow: " + String(WxForecast[r].Low));
      WxForecast[r].High              = list[r]["main"]["temp_max"].as<float>();          Serial.println("THig: " + String(WxForecast[r].High));
      WxForecast[r].Pressure          = list[r]["main"]["pressure"].as<float>();          Serial.println("Pres: " + String(WxForecast[r].Pressure));
      WxForecast[r].Humidity          = list[r]["main"]["humidity"].as<float>();          Serial.println("Humi: " + String(WxForecast[r].Humidity));
      //WxForecast[r].Forecast0         = list[r]["weather"][0]["main"].as<char*>();        Serial.println("For0: " + String(WxForecast[r].Forecast0));
      //WxForecast[r].Forecast1         = list[r]["weather"][1]["main"].as<char*>();        Serial.println("For1: " + String(WxForecast[r].Forecast1));
      //WxForecast[r].Forecast2         = list[r]["weather"][2]["main"].as<char*>();        Serial.println("For2: " + String(WxForecast[r].Forecast2));
      WxForecast[r].Icon              = list[r]["weather"][0]["icon"].as<const char*>();        Serial.println("Icon: " + String(WxForecast[r].Icon));
      //WxForecast[r].Description       = list[r]["weather"][0]["description"].as<char*>(); Serial.println("Desc: " + String(WxForecast[r].Description));
      //WxForecast[r].Cloudcover        = list[r]["clouds"]["all"].as<int>();               Serial.println("CCov: " + String(WxForecast[r].Cloudcover)); // in % of cloud cover
      //WxForecast[r].Windspeed         = list[r]["wind"]["speed"].as<float>();             Serial.println("WSpd: " + String(WxForecast[r].Windspeed));
      //WxForecast[r].Winddir           = list[r]["wind"]["deg"].as<float>();               Serial.println("WDir: " + String(WxForecast[r].Winddir));
      WxForecast[r].Rainfall          = list[r]["rain"]["3h"].as<float>();                Serial.println("Rain: " + String(WxForecast[r].Rainfall));
      WxForecast[r].Snowfall          = list[r]["snow"]["3h"].as<float>();                Serial.println("Snow: " + String(WxForecast[r].Snowfall));
      WxForecast[r].Period            = list[r]["dt_txt"].as<const char*>();                    Serial.println("Peri: " + String(WxForecast[r].Period));
    }
    // Measure pressure slope between ~now and later
    float pressure_trend = WxForecast[0].Pressure - WxForecast[2].Pressure;
    // Remove any small variations less than 0.1
    pressure_trend = ((int)(pressure_trend * 10)) / 10.0;
    WxConditions[0].Trend = "=";
    if (pressure_trend > 0)  WxConditions[0].Trend = "+";
    if (pressure_trend < 0)  WxConditions[0].Trend = "-";
    if (pressure_trend == 0) WxConditions[0].Trend = "0";
    //Convert to imperial units
    if (Units == "I") Convert_Readings_to_Imperial();
  }
  return true;
}

// DECODE METEROLOGICAL DATA FROM OPEN WEATHER MAP API
bool obtainWeatherData(WiFiClient & client, const String & RequestType) {
  const String units = (Units == "M" ? "metric" : "imperial");
  // Close connection before sending a new request
  client.stop();
  HTTPClient http;
  String uri = "/data/2.5/" + RequestType + "?q=" + City + "," + Country + "&APPID=" + apikey + "&mode=json&units=" + units + "&lang=" + Language;
  if (RequestType != "weather")
  {
    uri += "&cnt=" + String(max_readings);
  }
  // HTTPS example connection: http.begin(uri,test_root_ca);
  http.begin(client, server, 80, uri);
  int httpCode = http.GET();
  if (httpCode == HTTP_CODE_OK) {
    if (!DecodeWeather(http.getStream(), RequestType)) return false;
    client.stop();
    http.end();
    return true;
  }
  else
  {
    Serial.printf("connection failed, error: %s", http.errorToString(httpCode).c_str());
    client.stop();
    http.end();
    return false;
  }
  http.end();
  return true;
}

// DRAW VISIBILITY SYMBOL WITH TEXT
void Visibility(int x, int y, String Visi) {
  float start_angle = 0.52, end_angle = 2.61, Offset = 8;
  int r = 14;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    drawPixel(x + r * cos(i), y - r / 2 + r * sin(i) + Offset, Black);
    drawPixel(x + r * cos(i), 1 + y - r / 2 + r * sin(i) + Offset, Black);
  }
  start_angle = 3.61; end_angle = 5.78;
  for (float i = start_angle; i < end_angle; i = i + 0.05) {
    drawPixel(x + r * cos(i), y + r / 2 + r * sin(i) + Offset, Black);
    drawPixel(x + r * cos(i), 1 + y + r / 2 + r * sin(i) + Offset, Black);
  }
  fillCircle(x, y + Offset, r / 4, Black);
  drawString(x + 20, y, Visi, LEFT);
}

// DRAW ARROW
void arrow(int x, int y, int asize, float aangle, int pwidth, int plength) {
  // Calculate the position on the X-axis
  float dx = (asize - 10) * cos((aangle - 90) * PI / 180) + x;
  // Calculate the position on the Y-axis
  float dy = (asize - 10) * sin((aangle - 90) * PI / 180) + y;
  float x1 = 0;         float y1 = plength;
  float x2 = pwidth / 2;  float y2 = pwidth / 2;
  float x3 = -pwidth / 2; float y3 = pwidth / 2;
  float angle = aangle * PI / 180 - 135;
  float xx1 = x1 * cos(angle) - y1 * sin(angle) + dx;
  float yy1 = y1 * cos(angle) + x1 * sin(angle) + dy;
  float xx2 = x2 * cos(angle) - y2 * sin(angle) + dx;
  float yy2 = y2 * cos(angle) + x2 * sin(angle) + dy;
  float xx3 = x3 * cos(angle) - y3 * sin(angle) + dx;
  float yy3 = y3 * cos(angle) + x3 * sin(angle) + dy;
  fillTriangle(xx1, yy1, xx3, yy3, xx2, yy2, Black);
}

// DRAW SUN SYMBOL
void addsun(int x, int y, int scale, bool IconSize) {
  int linesize = 5;
  fillRect(x - scale * 2, y, scale * 4, linesize, Black);
  fillRect(x, y - scale * 2, linesize, scale * 4, Black);
  drawLine(x - scale * 1.3, y - scale * 1.3, x + scale * 1.3, y + scale * 1.3, Black);
  drawLine(x - scale * 1.3, y + scale * 1.3, x + scale * 1.3, y - scale * 1.3, Black);
  if (IconSize == LargeIcon) {
    drawLine(1 + x - scale * 1.3, y - scale * 1.3, 1 + x + scale * 1.3, y + scale * 1.3, Black);
    drawLine(2 + x - scale * 1.3, y - scale * 1.3, 2 + x + scale * 1.3, y + scale * 1.3, Black);
    drawLine(3 + x - scale * 1.3, y - scale * 1.3, 3 + x + scale * 1.3, y + scale * 1.3, Black);
    drawLine(1 + x - scale * 1.3, y + scale * 1.3, 1 + x + scale * 1.3, y - scale * 1.3, Black);
    drawLine(2 + x - scale * 1.3, y + scale * 1.3, 2 + x + scale * 1.3, y - scale * 1.3, Black);
    drawLine(3 + x - scale * 1.3, y + scale * 1.3, 3 + x + scale * 1.3, y - scale * 1.3, Black);
  }
  fillCircle(x, y, scale * 1.3, White);
  fillCircle(x, y, scale, Black);
  fillCircle(x, y, scale - linesize, White);
}

// DRAW MOON SYMBOL
void addmoon(int x, int y, int scale, bool IconSize) {
  if (IconSize == LargeIcon) {
    fillCircle(x - 85, y - 100, uint16_t(scale * 0.8), Black);
    fillCircle(x - 57, y - 100, uint16_t(scale * 1.6), White);
  }
  else
  {
    fillCircle(x - 28, y - 37, uint16_t(scale * 1.0), Black);
    fillCircle(x - 20, y - 37, uint16_t(scale * 1.6), White);
  }
}

// DRAW CLOUDS SYMBOL (symbols are drawn on a relative 10x10grid and 1 scale unit = 1 drawing unit)
void addcloud(int x, int y, int scale, int linesize) {
  // Draw leftmost circle
  fillCircle(x - scale * 3, y, scale, Black);
  // Draw rightmost circle
  fillCircle(x + scale * 3, y, scale, Black);
  // Draw left middle upper circle
  fillCircle(x - scale, y - scale, scale * 1.4, Black);
  // Draw right middle upper circle
  fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75, Black);
  // Draw upper and lower lines
  fillRect(x - scale * 3 - 1, y - scale, scale * 6, scale * 2 + 1, Black);
  // Draw clear leftmost circle
  fillCircle(x - scale * 3, y, scale - linesize, White);
  // Draw clear rightmost circle
  fillCircle(x + scale * 3, y, scale - linesize, White);
  // Draw left middle upper circle
  fillCircle(x - scale, y - scale, scale * 1.4 - linesize, White);
  // Draw right middle upper circle
  fillCircle(x + scale * 1.5, y - scale * 1.3, scale * 1.75 - linesize, White);
  // Draw upper and lower lines
  fillRect(x - scale * 3 + 2, y - scale + linesize - 1, scale * 5.9, scale * 2 - linesize * 2 + 2, White);
}

// DRAW RAIN SYMBOL
void addrain(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) {
    setFont(OpenSans8B);
    drawString(x - 25, y + 12, "///////", LEFT);
  }
  else
  {
    setFont(OpenSans18B);
    drawString(x - 60, y + 25, "///////", LEFT);
  }
}

// DRAW STORM SYMBOL
void addtstorm(int x, int y, int scale) {
  y = y + scale / 2;
  for (int i = 0; i < 5; i++) {
    drawLine(x - scale * 4 + scale * i * 1.5 + 0, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 0, y + scale, Black);
    if (scale != Small) {
      drawLine(x - scale * 4 + scale * i * 1.5 + 1, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 1, y + scale, Black);
      drawLine(x - scale * 4 + scale * i * 1.5 + 2, y + scale * 1.5, x - scale * 3.5 + scale * i * 1.5 + 2, y + scale, Black);
    }
    drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 0, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 0, Black);
    if (scale != Small) {
      drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 1, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 1, Black);
      drawLine(x - scale * 4 + scale * i * 1.5, y + scale * 1.5 + 2, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5 + 2, Black);
    }
    drawLine(x - scale * 3.5 + scale * i * 1.4 + 0, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 0, y + scale * 1.5, Black);
    if (scale != Small) {
      drawLine(x - scale * 3.5 + scale * i * 1.4 + 1, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 1, y + scale * 1.5, Black);
      drawLine(x - scale * 3.5 + scale * i * 1.4 + 2, y + scale * 2.5, x - scale * 3 + scale * i * 1.5 + 2, y + scale * 1.5, Black);
    }
  }
}

// DRAW FOG SYMBOL
void addfog(int x, int y, int scale, int linesize, bool IconSize) {
  if (IconSize == SmallIcon) {
    y -= 10;
    linesize = 1;
  }
  for (int i = 0; i < 6; i++) {
    fillRect(x - scale * 3, y + scale * 1.5, scale * 6, linesize, Black);
    fillRect(x - scale * 3, y + scale * 2.0, scale * 6, linesize, Black);
    fillRect(x - scale * 3, y + scale * 2.5, scale * 6, linesize, Black);
  }
}

// DRAW SNOW SYMBOL
void addsnow(int x, int y, int scale, bool IconSize) {
  if (IconSize == SmallIcon) {
    setFont(OpenSans8B);
    drawString(x - 25, y + 15, "* * * *", LEFT);
  }
  else
  {
    setFont(OpenSans18B);
    drawString(x - 60, y + 30, "* * * *", LEFT);
  }
}

// DRAW CLOUD COVER SYMBOL
void CloudCover(int x, int y, int CCover) {
  // Draw main cloud
  addcloud(x, y + 10, Small * 0.6, 2);
  // Draw cloud top left
  addcloud(x - 9, y + 2, Small * 0.3, 2);
  // Draw cloud top right
  addcloud(x + 3, y - 2, Small * 0.3, 2);
  drawString(x + 20, y, String(CCover) + "%", LEFT);
}

// DRAW SUNNY SYMBOL
void Sunny(int x, int y, bool IconSize, String IconName) {
  int scale = Small, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  // Slide up the little sun icon
  else y = y - 3;
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  scale = scale * 1.6;
  addsun(x, y, scale, IconSize);
}

// DRAW MOSTLY SUNNY SYMBOL
void MostlySunny(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
}

// DRAW CLOUDY SYMBOL
void Cloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  // Draw main cloud
  addcloud(x, y, scale, linesize);
  // Draw cloud top right
  addcloud(x + 15, y - 22, scale / 2, linesize);
  // Draw cloud top left
  addcloud(x - 10, y - 18, scale / 2, linesize);
}

// DRAW MOSTLY CLOUDY SYMBOL
void MostlyCloudy(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
}

// DRAW RAIN SYMBOL
void Rain(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}

// DRAW EXCEPT RAIN SYMBOL
void ExpectRain(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}

// DRAW CHANCE RAIN SYMBOL
void ChanceRain(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addsun(x - scale * 1.8, y - scale * 1.8, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addrain(x, y, scale, IconSize);
}

// DRAW TSTORMS SYMBOL
void Tstorms(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addtstorm(x, y, scale);
}

// DRAW HAZE SYMBOL
void Haze(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addsun(x, y - 5, scale * 1.4, IconSize);
  addfog(x, y - 5, scale * 1.4, linesize, IconSize);
}

// DRAW FOG SYMBOL
void Fog(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addcloud(x, y - 5, scale, linesize);
  addfog(x, y - 5, scale, linesize, IconSize);
}

// DRAW SNOW SYMBOL
void Snow(int x, int y, bool IconSize, String IconName) {
  int scale = Small, linesize = 5, Offset = 10;
  if (IconSize == LargeIcon) {
    scale = Large;
    Offset = 35;
  }
  if (IconName.endsWith("n")) addmoon(x, y + Offset, scale, IconSize);
  addcloud(x, y, scale, linesize);
  addsnow(x, y, scale, IconSize);
}

// DRAW NO DATA SYMBOL
void Nodata(int x, int y, bool IconSize, String IconName) {
  if (IconSize == LargeIcon) setFont(OpenSans24B); else setFont(OpenSans12B);
  drawString(x - 3, y - 10, "?", CENTER);
}

// DEVICE INITIALIZATION
void InitialiseSystem() {
  StartTime = millis();
  Serial.begin(115200);
  while (!Serial);
  Serial.println(String(__FILE__) + "\nStarting...");
  epd_init();
  framebuffer = (uint8_t *)ps_calloc(sizeof(uint8_t), EPD_WIDTH * EPD_HEIGHT / 2);
  if (!framebuffer) Serial.println("Memory alloc failed!");
  memset(framebuffer, 0xFF, EPD_WIDTH * EPD_HEIGHT / 2);
}

// SET THE DEVICE TO DEEP SLEEP
void BeginSleep() {
  epd_poweroff_all();
  UpdateLocalTime();
  //Some ESP32 have a RTC that is too fast to maintain accurate time, so add an offset
  SleepTimer = (SleepDuration * 60 - ((CurrentMin % SleepDuration) * 60 + CurrentSec)) + Delta;
  // in Secs, 1000000LL converted to seconds as unit = 1 uSec
  esp_sleep_enable_timer_wakeup(SleepTimer * 1000000LL);
  Serial.println("Awake for : " + String((millis() - StartTime) / 1000.0, 3) + "-secs");
  Serial.println("Entering " + String(SleepTimer) + " (secs) of sleep time");
  Serial.println("Starting deep-sleep period...");
  // Sleep for e.g. 60 minutes (set in main.h file)
  esp_deep_sleep_start();
}

// END THE WI-FI CONNECTION AND TURN OFF THE MODULE
void StopWiFi() {
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  Serial.println("WiFi switched Off");
}

// DISPLAY STATUS INFORMATION (RSSI and battery symbol)
void DisplayStatusSection(int x, int y, int rssi) {
  setFont(OpenSans8B);
  DrawRSSI(x + 305, y + 15, rssi);
  DrawBattery(x + 150, y);
}

// DISPLAY MAIN INFORMATION (city, date and time)
void DisplayGeneralInfoSection() {
  setFont(OpenSans14B);
  drawString(400, 5, City, LEFT);
  setFont(OpenSans8B);
  drawString(390, 40, Date_str + " (" + Time_str + " hod.)", LEFT);
}

// DISPLAY INFORMATION ABOUT WIND (direction of the wind, wind speed and compass)
void DisplayDisplayWindSection(int x, int y, float angle, float windspeed, int Cradius) {
  // Set the position on the X-axis and Y-axis
  int shiftWindSectionX = 0;
  int shiftWindSectionY = - 10;
  // Show wind direction on outer circle of width and length
  arrow(x + shiftWindSectionX, y + shiftWindSectionY, Cradius - 22, angle, 18, 33);
  setFont(OpenSans8B);
  int dxo, dyo, dxi, dyi;
  // Draw compass circle
  drawCircle(x + shiftWindSectionX, y + shiftWindSectionY, Cradius, Black);
  // Draw compass circle
  drawCircle(x + shiftWindSectionX, y + shiftWindSectionY, Cradius + 1, Black);
  // Draw compass inner circle
  drawCircle(x + shiftWindSectionX, y + shiftWindSectionY, Cradius * 0.7, Black);
  for (float a = 0; a < 360; a = a + 22.5) {
    dxo = Cradius * cos((a - 90) * PI / 180);
    dyo = Cradius * sin((a - 90) * PI / 180);
    if (a == 45)  drawString(dxo + x + shiftWindSectionX + 15, dyo + y + shiftWindSectionY - 18, TXT_NE, CENTER);
    if (a == 135) drawString(dxo + x + shiftWindSectionX + 20, dyo + y + shiftWindSectionY - 2,  TXT_SE, CENTER);
    if (a == 225) drawString(dxo + x + shiftWindSectionX - 20, dyo + y + shiftWindSectionY - 2,  TXT_SW, CENTER);
    if (a == 315) drawString(dxo + x + shiftWindSectionX - 15, dyo + y + shiftWindSectionY - 18, TXT_NW, CENTER);
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    drawLine(dxo + x + shiftWindSectionX, dyo + y + shiftWindSectionY, dxi + x + shiftWindSectionX, dyi + y + shiftWindSectionY, Black);
    dxo = dxo * 0.7;
    dyo = dyo * 0.7;
    dxi = dxo * 0.9;
    dyi = dyo * 0.9;
    drawLine(dxo + x + shiftWindSectionX, dyo + y + shiftWindSectionY, dxi + x + shiftWindSectionX, dyi + y + shiftWindSectionY, Black);
  }
  drawString(x + shiftWindSectionX, y + shiftWindSectionY - Cradius - 20, TXT_N, CENTER);
  drawString(x + shiftWindSectionX, y + shiftWindSectionY + Cradius + 10, TXT_S, CENTER);
  drawString(x + shiftWindSectionX - Cradius - 15, y + shiftWindSectionY - 5, TXT_W, CENTER);
  drawString(x + shiftWindSectionX + Cradius + 15, y + shiftWindSectionY - 5, TXT_E, CENTER);
  setFont(OpenSans10B);
  drawString(x + shiftWindSectionX, y + shiftWindSectionY - 50, WindDegToOrdinalDirection(angle), CENTER);
  drawString(x + shiftWindSectionX - 30, y + shiftWindSectionY - 9, String(windspeed, 1) + " m/s", LEFT);
  drawString(x + shiftWindSectionX + 3, y + shiftWindSectionY + 30, String(angle, 0) + " °", CENTER);
}

// DISPLAY ASTRONOMY ELEMENTS (sunrise time, sunset time and current moon phase)
void DisplayAstronomySection(int x, int y) {
  setFont(OpenSans10);
  // Display sunrise and sunset information
  DrawSunrise(x + 15, y + 30);
  drawString (x + 80, y + 40, ConvertUnixTime(WxConditions[0].Sunrise).substring(0, 5), LEFT);
  DrawSunset(x + 15, y + 80);
  drawString (x + 80, y + 89, ConvertUnixTime(WxConditions[0].Sunset).substring(0, 5), LEFT);
  time_t now = time(NULL);
  struct tm * now_utc  = gmtime(&now);
  const int day_utc    = now_utc->tm_mday;
  const int month_utc  = now_utc->tm_mon + 1;
  const int year_utc   = now_utc->tm_year + 1900;
  // Draw moon phase information
  setFont(OpenSans8B);
  drawString(x + 160, y + 88, MoonPhase(day_utc, month_utc, year_utc, Hemisphere), LEFT);
  DrawMoon(x + 160, y - 40, day_utc, month_utc, year_utc, Hemisphere);
}

// DISPLAY TEMPERATURE AND HUMIDITY INFORMATION
void DisplayTemperatureSection(int x, int y) {
  setFont(OpenSans16B);
  drawString(x - 30, y + 8, "Teplota: " + String(WxConditions[0].Temperature, 1) + " °C", LEFT);
  setFont(OpenSans10);
  drawString(x - 30, y + 60, "min / max:   " + String(WxConditions[0].High, 0) + " °C / " + String(WxConditions[0].Low, 0) + " °C", LEFT);
  setFont(OpenSans16B);
  drawString(x - 30, y + 100, "Vlhkost: " + String(WxConditions[0].Humidity, 0) + " %", LEFT);
}

// DISPLAY PRESSURE INFORMATION (pressure trend, pressure, visibility and cloud cover)
void DisplayPressureSection(int x, int y, float pressure, String slope) {
  setFont(OpenSans10B);
  DrawPressureAndTrend(x + 255, y - 100, pressure, slope);
  if (WxConditions[0].Visibility > 0) {
    Visibility(x + 270, y - 70, String(WxConditions[0].Visibility) + " m");
  }
  if (WxConditions[0].Cloudcover > 0) CloudCover(x + 270, y - 23, WxConditions[0].Cloudcover);
}

// DISPLAY THE WEATHER ICONS
void DisplayConditionsSection(int x, int y, String IconName, bool IconSize) {
  Serial.println("Icon name: " + IconName);
  if      (IconName == "01d" || IconName == "01n")  Sunny(x, y, IconSize, IconName);
  else if (IconName == "02d" || IconName == "02n")  MostlySunny(x, y, IconSize, IconName);
  else if (IconName == "03d" || IconName == "03n")  Cloudy(x, y, IconSize, IconName);
  else if (IconName == "04d" || IconName == "04n")  MostlySunny(x, y, IconSize, IconName);
  else if (IconName == "09d" || IconName == "09n")  ChanceRain(x, y, IconSize, IconName);
  else if (IconName == "10d" || IconName == "10n")  Rain(x, y, IconSize, IconName);
  else if (IconName == "11d" || IconName == "11n")  Tstorms(x, y, IconSize, IconName);
  else if (IconName == "13d" || IconName == "13n")  Snow(x, y, IconSize, IconName);
  else if (IconName == "50d")                       Haze(x, y, IconSize, IconName);
  else if (IconName == "50n")                       Fog(x, y, IconSize, IconName);
  else                                              Nodata(x, y, IconSize, IconName);
}

// DISPLAY THE FORECAST WEATHER
void DisplayForecastWeather(int x, int y, int index) {
  int fwidth = 90;
  x = x + fwidth * index;
  DisplayConditionsSection(x + fwidth / 2, y + 90, WxForecast[index].Icon, SmallIcon);
  setFont(OpenSans10B);
  drawString(x + fwidth / 2, y + 30, String(ConvertUnixTime(WxForecast[index].Dt + WxConditions[0].Timezone).substring(0, 5)), CENTER);
  drawString(x + fwidth / 2, y + 125, String(WxForecast[index].High, 0) + "°/" + String(WxForecast[index].Low, 0) + "°", CENTER);
}

// DISPLAY THE FORECAST WEATHER (including a graph of temperature, pressure, humidity and precipitation)
void DisplayForecastSection(int x, int y) {
  int f = 0;
  do {
    DisplayForecastWeather(x, y, f);
    f++;
  } while (f < max_readings);
  int r = 0;
  // Pre-load temporary arrays with with data - because C parses by reference and remember that[1] has already been converted to I units
  do {
    if (Units == "I") pressure_readings[r] = WxForecast[r].Pressure * 0.02953;   else pressure_readings[r] = WxForecast[r].Pressure;
    if (Units == "I") rain_readings[r]     = WxForecast[r].Rainfall * 0.0393701; else rain_readings[r]     = WxForecast[r].Rainfall;
    if (Units == "I") snow_readings[r]     = WxForecast[r].Snowfall * 0.0393701; else snow_readings[r]     = WxForecast[r].Snowfall;
    temperature_readings[r]                = WxForecast[r].Temperature;
    humidity_readings[r]                   = WxForecast[r].Humidity;
    r++;
  } while (r < max_readings);
  int gwidth = 175, gheight = 100;
  int gx = (SCREEN_WIDTH - gwidth * 4) / 5 + 8;
  int gy = (SCREEN_HEIGHT - gheight - 30);
  int gap = gwidth + gx;
  // x,y,width,height,MinValue, MaxValue, Title, Data Array, AutoScale, ChartMode
  DrawGraph(gx + 0 * gap, gy, gwidth, gheight, 10, 30,    Units == "M" ? TXT_TEMPERATURE_C : TXT_TEMPERATURE_F, temperature_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 1 * gap, gy, gwidth, gheight, 900, 1050, Units == "M" ? TXT_PRESSURE_HPA : TXT_PRESSURE_IN, pressure_readings, max_readings, autoscale_on, barchart_off);
  DrawGraph(gx + 2 * gap, gy, gwidth, gheight, 0, 100,   TXT_HUMIDITY_PERCENT, humidity_readings, max_readings, autoscale_off, barchart_off);
  if (SumOfPrecip(rain_readings, max_readings) >= SumOfPrecip(snow_readings, max_readings))
    DrawGraph(gx + 3 * gap + 5, gy, gwidth, gheight, 0, 30, Units == "M" ? TXT_RAINFALL_MM : TXT_RAINFALL_IN, rain_readings, max_readings, autoscale_on, barchart_on);
  else
    DrawGraph(gx + 3 * gap + 5, gy, gwidth, gheight, 0, 30, Units == "M" ? TXT_SNOWFALL_MM : TXT_SNOWFALL_IN, snow_readings, max_readings, autoscale_on, barchart_on);
}

// DISPLAY A TEXT DESCRIPTION OF THE WEATHER
void DisplayForecastTextSection(int x, int y) {
#define lineWidth 34
  setFont(OpenSans12B);
  // e.g. typically 'Clouds'
  //Wx_Description = WxConditions[0].Main0;
  // e.g. typically 'overcast clouds' => you choose which
  String Wx_Description = WxConditions[0].Forecast0;
  // remove any '.'
  Wx_Description.replace(".", "");
  int spaceRemaining = 0, p = 0, charCount = 0, Width = lineWidth;
  while (p < Wx_Description.length()) {
    if (Wx_Description.substring(p, p + 1) == " ") spaceRemaining = p;
    // '~' is the end of line marker
    if (charCount > Width - 1) { 
      Wx_Description = Wx_Description.substring(0, spaceRemaining) + "~" + Wx_Description.substring(spaceRemaining + 1);
      charCount = 0;
    }
    p++;
    charCount++;
  }
  if (WxForecast[0].Rainfall > 0) Wx_Description += " (" + String(WxForecast[0].Rainfall, 1) + String((Units == "M" ? "mm" : "in")) + ")";
  //Wx_Description = wordWrap(Wx_Description, lineWidth);
  String Line1 = Wx_Description.substring(0, Wx_Description.indexOf("~"));
  String Line2 = Wx_Description.substring(Wx_Description.indexOf("~") + 1);
  //drawString(x + 30, y + 80, TitleCase(Line1), LEFT);
  if (Line1 != Line2) drawString(x + 30, y + 30, Line2, LEFT);
}

// DISPLAY MAIN WEATHER INFORMATION
void DisplayMainWeatherSection(int x, int y) {
  setFont(OpenSans8B);
  DisplayTemperatureSection(x, y - 40);
  DisplayForecastTextSection(x - 55, y + 25);
  DisplayPressureSection(x - 25, y + 90, WxConditions[0].Pressure, WxConditions[0].Trend);
}

// DISPLAY WEATHER ICONS
void DisplayWeatherIcon(int x, int y) {
  DisplayConditionsSection(x, y, WxConditions[0].Icon, LargeIcon);
}

// DISPLAY WEATHER
void DisplayWeather() {
  DisplayStatusSection(600, 20, wifi_signal);
  DisplayGeneralInfoSection();
  DisplayDisplayWindSection(137, 150, WxConditions[0].Winddir, WxConditions[0].Windspeed, 100);
  DisplayAstronomySection(5, 255);
  DisplayMainWeatherSection(320, 110);
  DisplayWeatherIcon(810, 130);
  DisplayForecastSection(320, 220);
}

// UPDATE THE SCREEN
void edp_update() {
  epd_draw_grayscale_image(epd_full_screen(), framebuffer);
}

// START OF THE PROGRAM
void setup() {
  InitialiseSystem();
  if (StartWiFi() == WL_CONNECTED && SetupTime() == true) {
    bool WakeUp = false;                
    if (WakeupHour > SleepHour)
      WakeUp = (CurrentHour >= WakeupHour || CurrentHour <= SleepHour); 
    else                             
      WakeUp = (CurrentHour >= WakeupHour && CurrentHour <= SleepHour);                              
    if (WakeUp) {
      byte Attempts = 1;
      bool RxWeather  = false;
      bool RxForecast = false;
      WiFiClient client;
      // Try up-to 2 time for Weather and Forecast data
      while ((RxWeather == false || RxForecast == false) && Attempts <= 2) {
        if (RxWeather  == false) RxWeather  = obtainWeatherData(client, "weather");
        if (RxForecast == false) RxForecast = obtainWeatherData(client, "forecast");
        Attempts++;
      }
      Serial.println("Received all weather data...");
      // Only if received both Weather or Forecast proceed
      if (RxWeather && RxForecast) {
        // Turn off the Wi-Fi module (reduces power consumption)
        StopWiFi();
        // Switch on EPD display
        epd_poweron();
        // Clear the screen
        epd_clear();
        // Display the weather data
        DisplayWeather();
        // Update the display to show the information
        edp_update();
        // Switch off all power to EPD display
        epd_poweroff_all();
      }
    }
  }
  // Switch the display to sleep mode
  BeginSleep();
}

void loop() {
  // Nothing to do here
}